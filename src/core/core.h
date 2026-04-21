//------------------------------------------------------------------------------
// Core module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> def: _POSIX_C_SOURCE=200809L
//> def: _GNU_SOURCE

#pragma once

//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// INDEX
//
// [Config]             Configuration macros and settings
// [Macros]             Basic macros
// [Types]              Basic types and definitions
// [Library]            Library initialisation and shutdown
// [Memory]             Memory management functions
// [Array]              Dynamic array implementation
// [Mutex]              Simple locking for resource protection
// [Output]             Basic output to stdout and stderr
// [Arena]              Memory management via arenas and paging
// [Time]               Various cross-platform functions for handling time
// [Random]             Some simple routines for random number generation
// [String]             String views and builder
// [Map]                Dynamic hashmap implementation
// [Hash]               Hashing
// [FileMap]            Simple file-mapped routines
// [Path]               Path manipulation helpers
// [Directory]          Directory scanning helpers
// [Shell]              Run external commands
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------[Config]

#define YES (1)
#define NO (0)

//
// Compiler detection
//

#define COMPILER_GCC NO
#define COMPILER_CLANG NO
#define COMPILER_MSVC NO

#if defined(__clang__)
#    undef COMPILER_CLANG
#    define COMPILER_CLANG YES
#elif defined(__GNUC__)
#    undef COMPILER_GCC
#    define COMPILER_GCC YES
#elif defined(_MSC_VER)
#    undef COMPILER_MSVC
#    define COMPILER_MSVC YES
#else
#    error "Unsupported compiler. Please use GCC, Clang, or MSVC."
#endif

//
// OS detection
//

#define OS_WINDOWS NO
#define OS_LINUX NO
#define OS_MACOS NO
#define OS_BSD NO
#define OS_POSIX NO

#if defined(_WIN32) || defined(_WIN64)
#    undef OS_WINDOWS
#    define OS_WINDOWS YES
#elif defined(__linux__)
#    undef OS_LINUX
#    define OS_LINUX YES
#elif defined(__APPLE__) || defined(__MACH__)
#    undef OS_MACOS
#    define OS_MACOS YES
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#    undef OS_BSD
#    define OS_BSD YES
#else
#    error "Unsupported OS. Please use Windows, Linux, macOS, or BSD."
#endif

#if OS_LINUX || OS_MACOS || OS_BSD
#    undef OS_POSIX
#    define OS_POSIX YES
#endif

//
// Architecture detection
//

#define ARCH_X86 NO
#define ARCH_X86_64 NO
#define ARCH_ARM NO
#define ARCH_ARM64 NO

#if defined(__i386__) || defined(_M_IX86)
#    undef ARCH_X86
#    define ARCH_X86 YES
#elif defined(__x86_64__) || defined(_M_X64)
#    undef ARCH_X86_64
#    define ARCH_X86_64 YES
#elif defined(__arm__) || defined(_M_ARM)
#    undef ARCH_ARM
#    define ARCH_ARM YES
#elif defined(__aarch64__) || defined(_M_ARM64)
#    undef ARCH_ARM64
#    define ARCH_ARM64 YES
#else
#    error "Unsupported architecture. Please use x86, x86_64, ARM, or ARM64."
#endif

//
// Build configuration detection
//

#define CONFIG_DEBUG NO
#define CONFIG_RELEASE NO
#define CONFIG_PROFILE NO

#if defined(NDEBUG)
#    undef CONFIG_RELEASE
#    define CONFIG_RELEASE YES
#elif defined(FINAL)
#    undef CONFIG_PROFILE
#    define CONFIG_PROFILE YES
#else
#    undef CONFIG_DEBUG
#    define CONFIG_DEBUG YES
#endif

//
// Debugger support
//

#if COMPILER_MSVC
#    define DEBUG_BREAK() __debugbreak()
#elif COMPILER_GCC || COMPILER_CLANG
#    define DEBUG_BREAK() __builtin_trap()
#else
#    error                                                                     \
        "Unsupported compiler for debug break. Please use GCC, Clang, or MSVC."
#endif

//
// Standard includes
//

#if OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <windows.h>
#endif // OS_WINDOWS

#if OS_POSIX
#    include <dirent.h>
#    include <unistd.h>
#endif // OS_POSIX

//------------------------------------------------------------------------------[Macros]

#define internal static
#define global_variable static
#define local_persist static

#define UNUSED(x) (void)(x)

#define KB(x) ((x) * 1024ull)
#define MB(x) (KB(x) * 1024ull)
#define GB(x) (MB(x) * 1024ull)

#define ALIGN_UP(value, alignment)                                             \
    (((value) + ((alignment) - 1)) & ~((alignment) - 1))

#define ALIGN_PTR_UP(type, ptr, alignment)                                     \
    (type*)ALIGN_UP((usize)(ptr), (alignment))

#define PI 3.14159265358979323846
#define TAU (PI * 2.0)

#if CONFIG_DEBUG
#    define ASSERT(condition, ...)                                             \
        do {                                                                   \
            if (!(condition)) {                                                \
                eprn("ASSERTION FAILED: " #condition);                         \
                eprn(__VA_ARGS__);                                             \
                DEBUG_BREAK();                                                 \
                exit(1);                                                       \
            }                                                                  \
        } while (0)
#    define VERIFY(condition, ...) ASSERT(condition, __VA_ARGS__)
#else
#    define ASSERT(condition, ...)
#    define VERIFY(condition, ...) (condition)
#endif // CONFIG_DEBUG

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min, max) MAX((min), MIN((x), (max)))

#define DEF_SLICE(type)                                                        \
    typedef struct {                                                           \
        type* data;                                                            \
        usize count;                                                           \
    }

#define START_PACKED_STRUCT(name)                                              \
    typedef struct __attribute__((packed)) name##_t {
#define END_PACKED_STRUCT(name)                                                \
    }                                                                          \
    name;

#define STR_(x) #x
#define STR(x) STR_(x)

//------------------------------------------------------------------------------[Types]

typedef uint8_t  u8;  // Unsigned 8-bit integer
typedef uint16_t u16; // Unsigned 16-bit integer
typedef uint32_t u32; // Unsigned 32-bit integer
typedef uint64_t u64; // Unsigned 64-bit integer

typedef int8_t  i8;  // Signed 8-bit integer
typedef int16_t i16; // Signed 16-bit integer
typedef int32_t i32; // Signed 32-bit integer
typedef int64_t i64; // Signed 64-bit integer

typedef size_t    usize; // Unsigned size type (platform-dependent)
typedef ptrdiff_t isize; // Signed size type (platform-dependent)

typedef float  f32; // 32-bit floating point
typedef double f64; // 64-bit floating point

typedef const char* cstr; // Constant string type

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

//------------------------------------------------------------------------------[Library]

void kill(cstr format, ...);

//------------------------------------------------------------------------------[Memory]

void* mem_alloc(usize size, cstr file, int line);
void* mem_realloc(void* ptr, usize size, cstr file, int line);
void* mem_free(void* ptr, cstr file, int line);

usize mem_size(const void* ptr);
void  mem_leak(void* ptr);

void mem_check(void* ptr);

// Memory debugging utilities
#if CONFIG_DEBUG
void  mem_print_leaks(void);
usize mem_get_allocation_count(void);
usize mem_get_total_allocated(void);
#endif // CONFIG_DEBUG

#define ALLOC(size) mem_alloc((size), __FILE__, __LINE__)
#define REALLOC(ptr, size) mem_realloc((ptr), (size), __FILE__, __LINE__)
#define FREE(ptr) ptr = mem_free((ptr), __FILE__, __LINE__), (ptr) = NULL

#define ARRAY_ALLOC(type, count)                                               \
    (type*)mem_alloc(sizeof(type) * (count), __FILE__, __LINE__)
#define ARRAY_REALLOC(ptr, type, count)                                        \
    (type*)mem_realloc((ptr), sizeof(type) * (count), __FILE__, __LINE__)
#define ARRAY_FREE(ptr) FREE(ptr)

void mem_break_on_alloc(u64 index);

//------------------------------------------------------------------------------[Array]

#define Array(T) T*

typedef struct ArrayHeader_t {
    usize count;
} ArrayHeader;

void* array_maybe_grow(void* array,
                       usize element_size,
                       usize required_capacity,
                       cstr  file,
                       int   line);

// Level 0 accessor macros - assumes that (a) is non-NULL and is a valid array
#define __array_info(a) ((ArrayHeader*)(a) - 1)
#define __array_bytes_capacity(a)                                              \
    (mem_size(__array_info(a)) - sizeof(ArrayHeader))
#define __array_count(a) (__array_info(a)->count)
#define __array_bytes_size(a) (__array_count(a) * sizeof(*(a)))
#define __array_safe(a, op) ((a) ? (op) : 0)

// Level 1 accessor macros - handles a NULL array
#define array_size(a) __array_safe((a), __array_bytes_size(a))
#define array_capacity(a)                                                      \
    __array_safe((a), __array_bytes_capacity(a) / sizeof(*(a)))
#define array_count(a) __array_safe((a), __array_count(a))

#define array_push(a, ...)                                                     \
    do {                                                                       \
        typeof(*(a)) __array_tmp[] = {__VA_ARGS__};                            \
        usize        __array_n = sizeof(__array_tmp) / sizeof(__array_tmp[0]); \
        (a)                    = array_maybe_grow((a),                         \
                               sizeof(*(a)),                \
                               array_count(a) + __array_n,  \
                               __FILE__,                    \
                               __LINE__);                   \
        memcpy((a) + __array_count(a), __array_tmp, __array_n * sizeof(*(a))); \
        __array_count(a) += __array_n;                                         \
    } while (0)

#define array_pop(a) ((a)[__array_count(a)-- - 1])

// Array free function - deallocates array memory
#define array_free(a)                                                          \
    do {                                                                       \
        if ((a)) {                                                             \
            ArrayHeader* __header = __array_info(a);                           \
            FREE(__header);                                                    \
            (a) = NULL;                                                        \
        }                                                                      \
    } while (0)

#define array_delete(a, index)                                                 \
    do {                                                                       \
        usize __array_index = (index);                                         \
        if (__array_index < array_count(a)) {                                  \
            memmove(&(a)[__array_index],                                       \
                    &(a)[__array_index + 1],                                   \
                    (array_count(a) - __array_index - 1) * sizeof(*(a)));      \
            __array_count(a)--;                                                \
        }                                                                      \
    } while (0)

#define array_clear(a)                                                         \
    do {                                                                       \
        if (a) {                                                               \
            __array_count(a) = 0;                                              \
        }                                                                      \
    } while (0)

// Array ensure capacity macro
#define array_requires_capacity(a, required_capacity)                          \
    (a) = (typeof(*(a))*)array_maybe_grow(                                     \
        (a), sizeof(*(a)), (required_capacity), __FILE__, __LINE__)

// Ensure capacity incrementally
#define array_more_capacity(a, additional_capacity)                            \
    (a) = (typeof(*(a))*)array_maybe_grow((a),                                 \
                                          sizeof(*(a)),                        \
                                          array_count(a) +                     \
                                              (additional_capacity),           \
                                          __FILE__,                            \
                                          __LINE__)

// Array ensure size and capacity macro
#define array_requires_size(a, required_size)                                  \
    do {                                                                       \
        (a) = (typeof(*(a))*)array_maybe_grow(                                 \
            (a), sizeof(*(a)), (required_size), __FILE__, __LINE__);           \
        __array_count(a) = (required_size);                                    \
    } while (0)

#define array_more_size(a, additional_size)                                    \
    do {                                                                       \
        (a) = (typeof(*(a))*)array_maybe_grow((a),                             \
                                              sizeof(*(a)),                    \
                                              array_count(a) +                 \
                                                  (additional_size),           \
                                              __FILE__,                        \
                                              __LINE__);                       \
        __array_count(a) += (additional_size);                                 \
    } while (0)

#define array_leak(a) mem_leak(__array_info(a))

//------------------------------------------------------------------------------[Arena]

#define ARENA_DEFAULT_NUM_PAGES_GROW 16

// OS-based arena with reserved memory pages
typedef struct {
    u8*   data;           // Base pointer to arena - never changes
    usize cursor;         // Current allocation cursor
    usize committed_size; // Number of bytes currently committed
    usize reserved_size;  // Total number of bytes reserved (maximum capacity)
    usize alloc_granularity; // OS allocation granularity (page size)
    usize grow_rate;         // Number of pages to grow by when expanding
} Arena;

// Used to build arrays within an arena
typedef struct {
    Arena* arena;        // Arena being used
    usize  count;        // Number of elements currently in array
    usize  alignment;    // Alignment of each element
    usize  element_size; // Size of each element
    void*  start;        // Start of the array in the arena
} ArenaSession;

//
// Arena Lifetime
//

typedef struct {
    usize reserved_size;
    usize grow_rate;
} ArenaDefaultParams;

void _arena_init(Arena* arena, ArenaDefaultParams params);

#define arena_init(arena, ...)                                                 \
    _arena_init((arena), (ArenaDefaultParams){__VA_ARGS__})

void arena_done(Arena* arena);

//
// Arena allocation
//

void* arena_alloc(Arena* arena, usize size);
void  arena_align(Arena* arena, usize align);
void* arena_alloc_align(Arena* arena, usize size, usize align);

u8*  arena_formatv(Arena* arena, cstr fmt, va_list args);
u8*  arena_format(Arena* arena, cstr fmt, ...);
void arena_null_terminate(Arena* arena);

//
// Arena marks
//

void* arena_store(Arena* arena);
void  arena_restore(Arena* arena, void* mark);
void  arena_reset(Arena* arena);

//
// Arena state
//

u32 arena_offset(Arena* arena, void* p);

//
// Arena sessions
//

void arena_session_init(ArenaSession* session,
                        Arena*        arena,
                        usize         alignment,
                        usize         element_size);
void arena_session_undo(ArenaSession* session);

void* arena_session_alloc(ArenaSession* session, usize count);
usize arena_session_count(ArenaSession* session);
void* arena_session_address(ArenaSession* session);

// Global arena for use any where
extern Arena temp_arena;

// Resets the global temp arena, undoing all allocations. Use with care - this
// will invalidate any pointers allocated from the temp arena.  Usually used
// within a main loop.
void temp_arena_reset(void);

//------------------------------------------------------------------------------[Mutex]

#if OS_WINDOWS
typedef CRITICAL_SECTION Mutex;
#elif OS_POSIX
#    include <pthread.h>
typedef pthread_mutex_t Mutex;
#else
#    error "Mutex not implemented for this OS."
#endif

void mutex_init(Mutex* mutex);
void mutex_done(Mutex* mutex);
void mutex_lock(Mutex* mutex);
void mutex_unlock(Mutex* mutex);

//------------------------------------------------------------------------------[Output]

void prv(const char* format, va_list args);
void pr(const char* format, ...);
void prn(const char* format, ...);
void eprv(const char* format, va_list args);
void epr(const char* format, ...);
void eprn(const char* format, ...);

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_FAINT "\033[2m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_INVERSED "\033[7m"

#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"

#define ANSI_BOLD_BLACK "\033[1;30m"
#define ANSI_BOLD_RED "\033[1;31m"
#define ANSI_BOLD_GREEN "\033[1;32m"
#define ANSI_BOLD_YELLOW "\033[1;33m"
#define ANSI_BOLD_BLUE "\033[1;34m"
#define ANSI_BOLD_MAGENTA "\033[1;35m"
#define ANSI_BOLD_CYAN "\033[1;36m"
#define ANSI_BOLD_WHITE "\033[1;37m"

#define ANSI_FAINT_BLACK "\033[2;30m"
#define ANSI_FAINT_RED "\033[2;31m"
#define ANSI_FAINT_GREEN "\033[2;32m"
#define ANSI_FAINT_YELLOW "\033[2;33m"
#define ANSI_FAINT_BLUE "\033[2;34m"
#define ANSI_FAINT_MAGENTA "\033[2;35m"
#define ANSI_FAINT_CYAN "\033[2;36m"
#define ANSI_FAINT_WHITE "\033[2;37m"

#define ANSI_BG_BLACK "\033[40m"
#define ANSI_BG_RED "\033[41m"
#define ANSI_BG_GREEN "\033[42m"
#define ANSI_BG_YELLOW "\033[43m"
#define ANSI_BG_BLUE "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN "\033[46m"
#define ANSI_BG_WHITE "\033[47m"

#define ANSI_BG_BOLD_BLACK "\033[1;40m"
#define ANSI_BG_BOLD_RED "\033[1;41m"
#define ANSI_BG_BOLD_GREEN "\033[1;42m"
#define ANSI_BG_BOLD_YELLOW "\033[1;43m"
#define ANSI_BG_BOLD_BLUE "\033[1;44m"
#define ANSI_BG_BOLD_MAGENTA "\033[1;45m"
#define ANSI_BG_BOLD_CYAN "\033[1;46m"
#define ANSI_BG_BOLD_WHITE "\033[1;47m"

#define ANSI_BG_FAINT_BLACK "\033[2;40m"
#define ANSI_BG_FAINT_RED "\033[2;41m"
#define ANSI_BG_FAINT_GREEN "\033[2;42m"
#define ANSI_BG_FAINT_YELLOW "\033[2;43m"
#define ANSI_BG_FAINT_BLUE "\033[2;44m"
#define ANSI_BG_FAINT_MAGENTA "\033[2;45m"
#define ANSI_BG_FAINT_CYAN "\033[2;46m"
#define ANSI_BG_FAINT_WHITE "\033[2;47m"

#define UNICODE_TREE_BRANCH "├─ "
#define UNICODE_TREE_LAST_BRANCH "└─ "
#define UNICODE_TREE_VERTICAL "│  "

#define UNICODE_TABLE_TOP_LEFT "┌"
#define UNICODE_TABLE_TOP_RIGHT "┐"
#define UNICODE_TABLE_BOTTOM_LEFT "└"
#define UNICODE_TABLE_BOTTOM_RIGHT "┘"
#define UNICODE_TABLE_HORIZONTAL "─"
#define UNICODE_TABLE_VERTICAL "│"
#define UNICODE_TABLE_T_LEFT "├"
#define UNICODE_TABLE_T_RIGHT "┤"
#define UNICODE_TABLE_T_TOP "┬"
#define UNICODE_TABLE_T_BOTTOM "┴"
#define UNICODE_TABLE_CROSS "┼"

//------------------------------------------------------------------------------[Time]

typedef u64 TimePoint;
typedef u64 TimeDuration;
typedef u64 ThreadTimePoint;

TimePoint       time_now(void);
TimeDuration    time_elapsed(TimePoint start, TimePoint end);
TimePoint       time_add_duration(TimePoint time, TimeDuration duration);
ThreadTimePoint thread_time_now(void);
TimeDuration    thread_time_elapsed(ThreadTimePoint start, ThreadTimePoint end);

void time_sleep_ms(u32 milliseconds);

u64 time_duration_to_secs(TimeDuration duration);
u64 time_duration_to_ms(TimeDuration duration);
u64 time_duration_to_us(TimeDuration duration);
u64 time_duration_to_ns(TimeDuration duration);

f64 time_secs(TimeDuration duration);
f64 time_msecs(TimeDuration duration);
f64 time_usecs(TimeDuration duration);
f64 time_nsecs(TimeDuration duration);

TimeDuration time_from_secs(u64 seconds);
TimeDuration time_from_ms(u64 milliseconds);
TimeDuration time_from_us(u64 microseconds);
TimeDuration time_from_ns(u64 nanoseconds);

void time_sleep(TimeDuration duration);

//------------------------------------------------------------------------------[Random]

void random_seed(u64 seed);
u64  random_u64(void);
u64  random_range_u64(u64 min, u64 max);
i64  random_range_i64(i64 min, i64 max);

//------------------------------------------------------------------------------[String]

DEF_SLICE(u8) string;
#define s(cstr) string_from_cstr(cstr)

// Macro to create pr string parameters from a string.
#define STRINGV(s) (int)((s).count), (s).data

// Macro to insert into a format string that needs to be paired with a STRINGV
#define STRINGP "%.*s"

#define SLICE_SET(slice_name, slice_type, ...)                                 \
    static slice_type slice_name##_data[] = {__VA_ARGS__};                     \
    DEF_SLICE(slice_type) slice_name##_t;                                      \
    slice_name##_t slice_name = {slice_name##_data,                            \
                                 sizeof(slice_name##_data) /                   \
                                     sizeof(slice_name##_data[0])}

//------------------------------------------------------------------------------
// String construction

string string_from_cstr(const char* cstr);
string string_formatv(Arena* arena, cstr fmt, va_list args);
string string_format(Arena* arena, cstr fmt, ...);
string string_from(u8* data, usize size);

bool string_eq(string a, string b);
bool string_eq_cstr(string a, cstr b);

//------------------------------------------------------------------------------
// StringBuilder API

typedef struct {
    u8*    data;
    usize  size;
    Arena* arena;
} StringBuilder;

void sb_init(StringBuilder* sb, Arena* arena);
void sb_append_cstr(StringBuilder* sb, cstr str);
void sb_append_string(StringBuilder* sb, string str);
void sb_append_char(StringBuilder* sb, char c);
void sb_append_null(StringBuilder* sb);
void sb_advance(StringBuilder* sb, usize count);
void sb_formatv(StringBuilder* sb, cstr fmt, va_list args);
void sb_format(StringBuilder* sb, cstr fmt, ...);

string sb_to_string(StringBuilder* sb);

//------------------------------------------------------------------------------[Map]

typedef struct {
    u64*    hashes;
    string* keys;
    u8*     values;
    usize   count;
    usize   value_size;
    Arena   key_arena;
    usize   key_bytes_live;
    usize   key_bytes_dead;
} Map;

typedef struct {
    usize index;
} MapIter;

static inline MapIter map_iter(void) { return (MapIter){0}; }
void                  map_iter_init(MapIter* iter);
bool  map_next(Map* map, MapIter* iter, string* out_key, void** out_value);
void  _map_init(Map* map, usize value_size, usize initial_capacity);
void  map_done(Map* map);
void  map_clear(Map* map);
bool  map_insert(Map* map, string key, const void* value);
bool  map_delete(Map* map, string key);
void* map_find(Map* map, string key);
void* map_entry(Map* map, string key, bool* out_created);

#define DEF_MAP(name, value_type)                                              \
    typedef struct {                                                           \
        Map map;                                                               \
    } name;                                                                    \
    static inline void name##_init(name* self, usize initial_capacity)         \
    {                                                                          \
        _map_init(&self->map, sizeof(value_type), initial_capacity);           \
    }                                                                          \
    static inline void name##_done(name* self) { map_done(&self->map); }       \
    static inline void name##_clear(name* self) { map_clear(&self->map); }     \
    static inline bool name##_insert(name* self, string key, value_type value) \
    {                                                                          \
        return map_insert(&self->map, key, &value);                            \
    }                                                                          \
    static inline bool name##_delete(name* self, string key)                   \
    {                                                                          \
        return map_delete(&self->map, key);                                    \
    }                                                                          \
    static inline value_type* name##_find(name* self, string key)              \
    {                                                                          \
        return (value_type*)map_find(&self->map, key);                         \
    }                                                                          \
    static inline usize name##_count(const name* self)                         \
    {                                                                          \
        return self->map.count;                                                \
    }                                                                          \
    static inline value_type* name##_entry(                                    \
        name* self, string key, bool* out_created)                             \
    {                                                                          \
        return (value_type*)map_entry(&self->map, key, out_created);           \
    }                                                                          \
    static inline MapIter name##_iter(void) { return map_iter(); }             \
    static inline bool    name##_next(                                         \
        name* self, MapIter* iter, string* out_key, value_type** out_value) \
    {                                                                          \
        void* value = NULL;                                                    \
        if (!map_next(&self->map, iter, out_key, &value)) {                    \
            return false;                                                      \
        }                                                                      \
        if (out_value != NULL) {                                               \
            *out_value = (value_type*)value;                                   \
        }                                                                      \
        return true;                                                           \
    }

//------------------------------------------------------------------------------[Hash]

u64 hash_fnv1a(const void* data, usize size);
u64 hash_fnv1a_string(string str);

//------------------------------------------------------------------------------[FileMap]

typedef struct {
    u8*   data;
    usize size;

#if OS_WINDOWS
// Windows-specific file-mapping state
#elif OS_POSIX
// POSIX-specific file-mapping state
#endif
} FileMap;

string filemap_load(cstr path, FileMap* filemap);
void   filemap_unload(FileMap* filemap);

//------------------------------------------------------------------------------[Path]

bool   path_exists(cstr path);
bool   path_is_directory(cstr path);
bool   path_has_extension(string path, cstr extension);
string path_filename(string path);
string path_stem(string path);
cstr   path_join(Arena* arena, cstr left, cstr right);
cstr   path_replace_extension(Arena* arena, cstr path, cstr extension);
bool   path_remove(cstr path);

//------------------------------------------------------------------------------[Directory]

typedef struct {
#if OS_WINDOWS
    HANDLE           handle;
    WIN32_FIND_DATAA find_data;
    bool             first_ready;
    char*            pattern;
#elif OS_POSIX
    DIR* dir;
#endif
    cstr path;
} DirIter;

bool dir_iter_init(DirIter* iter, cstr path);
bool dir_iter_next(DirIter* iter,
                   Arena*   arena,
                   cstr*    out_path,
                   bool*    out_is_directory);
void dir_iter_done(DirIter* iter);

//------------------------------------------------------------------------------[Shell]

typedef struct {
    int    exit_code;
    string stdout_text;
    string stderr_text;
} ShellResult;

int         shell(cstr command);
ShellResult shell_capture(cstr command, Arena* arena);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
