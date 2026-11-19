#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdatomic.h>
#if defined(_WIN32)
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

typedef uint8_t u8;

// Keep these primitives runtime-owned. LLVM may otherwise turn their loops back
// into libc calls, even when the source no longer mentions libc functions.
#if defined(__clang__)
#define NRT_NO_BUILTIN __attribute__((no_builtin))
#else
#define NRT_NO_BUILTIN
#endif

static NRT_NO_BUILTIN void nrt_bytes_copy(void* destination,
                                         const void* source,
                                         size_t count)
{
    u8* out = destination;
    const u8* in = source;
    for (size_t i = 0; i < count; ++i) out[i] = in[i];
}

static NRT_NO_BUILTIN void nrt_bytes_move(void* destination,
                                         const void* source,
                                         size_t count)
{
    u8* out = destination;
    const u8* in = source;
    if ((uintptr_t)out <= (uintptr_t)in) {
        for (size_t i = 0; i < count; ++i) out[i] = in[i];
    } else {
        while (count != 0) {
            --count;
            out[count] = in[count];
        }
    }
}

static NRT_NO_BUILTIN int nrt_bytes_compare(const void* left,
                                            const void* right,
                                            size_t count)
{
    const u8* a = left;
    const u8* b = right;
    for (size_t i = 0; i < count; ++i) {
        if (a[i] != b[i]) return (int)a[i] - (int)b[i];
    }
    return 0;
}

static NRT_NO_BUILTIN size_t nrt_cstr_count(const char* text)
{
    size_t count = 0;
    while (text[count] != '\0') ++count;
    return count;
}

static bool nrt_cstr_equal(const char* left, const char* right)
{
    size_t count = nrt_cstr_count(left);
    return count == nrt_cstr_count(right) &&
           nrt_bytes_compare(left, right, count) == 0;
}

// These are raw kernel results, not libc's -1/errno convention. Keep the bridge
// private: the public os.linux syscall API still has its existing error contract.
#if defined(__linux__) && (defined(__x86_64__) || defined(__aarch64__))
static intptr_t nrt_linux_syscall6(intptr_t number,
                                  uintptr_t a0, uintptr_t a1, uintptr_t a2,
                                  uintptr_t a3, uintptr_t a4, uintptr_t a5)
{
#if defined(__x86_64__)
    register uintptr_t r10 __asm__("r10") = a3;
    register uintptr_t r8 __asm__("r8") = a4;
    register uintptr_t r9 __asm__("r9") = a5;
    intptr_t result;
    __asm__ volatile("syscall"
                     : "=a"(result)
                     : "a"(number), "D"(a0), "S"(a1), "d"(a2),
                       "r"(r10), "r"(r8), "r"(r9)
                     : "rcx", "r11", "memory", "cc");
    return result;
#else
    register uintptr_t x0 __asm__("x0") = a0;
    register uintptr_t x1 __asm__("x1") = a1;
    register uintptr_t x2 __asm__("x2") = a2;
    register uintptr_t x3 __asm__("x3") = a3;
    register uintptr_t x4 __asm__("x4") = a4;
    register uintptr_t x5 __asm__("x5") = a5;
    register intptr_t x8 __asm__("x8") = number;
    __asm__ volatile("svc 0"
                     : "+r"(x0)
                     : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
                     : "memory", "cc");
    return (intptr_t)x0;
#endif
}

#if defined(__x86_64__)
enum {
    NRT_SYS_READ = 0, NRT_SYS_WRITE = 1, NRT_SYS_IOCTL = 16,
    NRT_SYS_MMAP = 9, NRT_SYS_MPROTECT = 10, NRT_SYS_MUNMAP = 11,
    NRT_SYS_EXIT_GROUP = 231
};
#else
enum {
    NRT_SYS_READ = 63, NRT_SYS_WRITE = 64, NRT_SYS_IOCTL = 29,
    NRT_SYS_MMAP = 222, NRT_SYS_MPROTECT = 226, NRT_SYS_MUNMAP = 215,
    NRT_SYS_EXIT_GROUP = 94
};
#endif
#define NRT_LINUX_RAW_SYSCALLS 1
#endif

// Dynamic-array payloads follow this header at a 16-byte boundary. Keep this
// layout aligned with the native backend's dynamic-array header.
typedef struct {
    _Alignas(16) void* data;
    size_t            count, capacity;
} NrtArray;

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
    NerdString                 source_path;
    size_t                     requested_size;
    uint64_t                   index;
    uint32_t                   line;
} NrtHeapDebugHeader;

typedef struct NrtArenaDebugNode {
    struct NrtArenaDebugNode* prev;
    struct NrtArenaDebugNode* next;
    void*                     base;
    size_t                    reserved_size;
    size_t                    committed_size;
    size_t                    used_size;
    NerdString                source_path;
    uint64_t                  index;
    uint32_t                  line;
    size_t                    mapping_size;
} NrtArenaDebugNode;

static NrtHeapDebugHeader* g_nrt_heap_head        = NULL;
static NrtArenaDebugNode*  g_nrt_arena_head       = NULL;
static uint64_t            g_nrt_heap_next_index  = 0;
static uint64_t            g_nrt_heap_break_index = 0;
static uint64_t            g_nrt_arena_next_index = 0;
// Protect process-wide diagnostic bookkeeping, not caller-owned payloads.
static atomic_flag g_nrt_debug_lock = ATOMIC_FLAG_INIT;
static void nrt_debug_lock(void)
{
    while (atomic_flag_test_and_set_explicit(&g_nrt_debug_lock, memory_order_acquire)) {}
}
static void nrt_debug_unlock(void)
{
    atomic_flag_clear_explicit(&g_nrt_debug_lock, memory_order_release);
}

#endif

void nrt_string_builder_reset(void);
void nrt_mem_free(void* memory);
size_t nrt_mem_size(void* memory);
void nrt_mem_leak(void* memory);
NrtArena* nrt_temp_arena(void);
void* nrt_arena_alloc(NrtArena*   arena,
                      size_t      size,
                      size_t      alignment,
                      const char* source_path,
                      uint32_t    line);
void nrt_thread_init(void);
void nrt_thread_done(void);
void nrt_core_init(void);
void nrt_core_done(void);

#if defined(_WIN32)
static bool nrt_windows_valid_handle(HANDLE handle)
{
    if (handle == NULL || handle == INVALID_HANDLE_VALUE) return false;
    SetLastError(NO_ERROR);
    return GetFileType(handle) != FILE_TYPE_UNKNOWN || GetLastError() == NO_ERROR;
}

static HANDLE nrt_windows_stream_handle(FILE* stream)
{
    // An unattached GUI process can have the special CRT descriptor -2.
    // Do not pass it to _get_osfhandle, which invokes the invalid-parameter handler.
    int fd = _fileno(stream);
    return fd >= 0 ? (HANDLE)_get_osfhandle(fd) : INVALID_HANDLE_VALUE;
}

static void nrt_windows_connect_stream(FILE* stream, HANDLE handle, bool input)
{
    if (nrt_windows_valid_handle(nrt_windows_stream_handle(stream)) ||
        !nrt_windows_valid_handle(handle)) return;

    HANDLE owned = NULL;
    if (!DuplicateHandle(GetCurrentProcess(), handle, GetCurrentProcess(),
                         &owned, 0, FALSE, DUPLICATE_SAME_ACCESS)) return;
    // Reopen only an unusable FILE. This gives the CRT's -2 placeholder a real
    // descriptor, then associates it with a separately owned copy of the handle.
    FILE* reopened = NULL;
    if (freopen_s(&reopened, "NUL", input ? "r" : "w", stream) != 0) {
        CloseHandle(owned);
        return;
    }
    int fd = _open_osfhandle((intptr_t)owned,
                            _O_TEXT | (input ? _O_RDONLY : _O_WRONLY));
    if (fd < 0) {
        CloseHandle(owned);
        return;
    }
    if (_dup2(fd, _fileno(stream)) == 0) clearerr(stream);
    _close(fd);
}

// Called only by the generated GUI entry point, before module initialization.
// Never create a console: Explorer launches must remain window-only.
void nrt_windows_init_stdio(void)
{
    const DWORD ids[] = {STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE};
    FILE* streams[] = {stdin, stdout, stderr};
    HANDLE inherited[3];
    bool missing = false;
    for (size_t i = 0; i < 3; ++i) {
        inherited[i] = GetStdHandle(ids[i]);
        if (!nrt_windows_valid_handle(inherited[i])) {
            inherited[i] = nrt_windows_stream_handle(streams[i]);
        }
        missing |= !nrt_windows_valid_handle(inherited[i]);
    }
    if (missing) AttachConsole(ATTACH_PARENT_PROCESS);

    for (size_t i = 0; i < 3; ++i) {
        HANDLE handle = inherited[i];
        if (nrt_windows_valid_handle(handle)) {
            // Attachment may replace the Win32 standard handle table. Preserve
            // each inherited redirection independently, including mixed output.
            SetStdHandle(ids[i], handle);
        } else {
            handle = GetStdHandle(ids[i]);
        }
        // STARTF_USESTDHANDLES can leave a missing entry empty even on attachment.
        bool opened = false;
        if (!nrt_windows_valid_handle(handle) && GetConsoleCP() != 0) {
            handle = CreateFileW(i == 0 ? L"CONIN$" : L"CONOUT$",
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING, 0, NULL);
            opened = nrt_windows_valid_handle(handle);
        }
        nrt_windows_connect_stream(streams[i], handle, i == 0);
        HANDLE connected = nrt_windows_stream_handle(streams[i]);
        if (nrt_windows_valid_handle(connected)) {
            SetStdHandle(ids[i], connected);
            DWORD mode;
            if (i != 0 && GetConsoleMode(connected, &mode)) {
                // The Windows CRT does not implement terminal line buffering.
                setvbuf(streams[i], NULL, _IONBF, 0);
            }
        }
        if (opened) CloseHandle(handle);
    }
}
#endif

// Linux runtime streams are descriptors, not libc FILE objects. Avoid bulk
// read-ahead: other APIs may also read descriptor zero (CR handling retains one
// pushback byte). Foreign FILE buffering is separate; callers mixing it with
// Nerd I/O must synchronise it themselves.
static void nrt_write_bytes(bool error_stream, const void* data, size_t count)
{
    if (data == NULL || count == 0) return;
#if defined(NRT_LINUX_RAW_SYSCALLS)
    const u8* bytes = data;
    while (count != 0) {
        size_t chunk = count > 0x7ffff000u ? 0x7ffff000u : count;
        intptr_t written = nrt_linux_syscall6(NRT_SYS_WRITE,
                                              error_stream ? 2 : 1,
                                              (uintptr_t)bytes, chunk, 0, 0, 0);
        if (written == -4) continue; // EINTR: no bytes transferred.
        if (written <= 0) return; // Includes EAGAIN, EBADF and ignored SIGPIPE.
        bytes += (size_t)written;
        count -= (size_t)written;
    }
#else
    fwrite(data, 1, count, error_stream ? stderr : stdout);
#endif
}

#if defined(NRT_LINUX_RAW_SYSCALLS)
typedef struct {
    u8 data[256];
    size_t count;
} NrtDiagnostic;

static void nrt_diagnostic_byte(NrtDiagnostic* output, u8 byte)
{
    output->data[output->count++] = byte;
    if (output->count == sizeof(output->data)) {
        nrt_write_bytes(true, output->data, output->count);
        output->count = 0;
    }
}

// Only the private runtime diagnostic vocabulary, not a printf replacement for
// user VaList.format: strings, unsigned decimals, widths, left alignment and %%.
static void nrt_diagnostic_format(const char* format, va_list args)
{
    NrtDiagnostic output;
    output.count = 0;
    while (*format != '\0') {
        if (*format != '%') {
            nrt_diagnostic_byte(&output, (u8)*format++);
            continue;
        }
        ++format;
        if (*format == '%') {
            nrt_diagnostic_byte(&output, '%');
            ++format;
            continue;
        }
        bool left = *format == '-';
        if (left) ++format;
        size_t width = 0;
        if (*format == '*') {
            int value = va_arg(args, int);
            if (value < 0) left = true;
            width = value < 0 ? (size_t)-(int64_t)value : (size_t)value;
            ++format;
        } else {
            while (*format >= '0' && *format <= '9') {
                width = width * 10 + (size_t)(*format++ - '0');
            }
        }
        enum { NRT_DIAG_INT, NRT_DIAG_SIZE, NRT_DIAG_LLONG } length = NRT_DIAG_INT;
        if (*format == 'z') {
            length = NRT_DIAG_SIZE;
            ++format;
        } else if (format[0] == 'l' && format[1] == 'l') {
            length = NRT_DIAG_LLONG;
            format += 2;
        }
        const u8* text;
        size_t count;
        u8 digits[20];
        if (*format == 's' && length == NRT_DIAG_INT) {
            const char* value = va_arg(args, const char*);
            text = (const u8*)(value != NULL ? value : "(null)");
            count = nrt_cstr_count((const char*)text);
        } else if (*format == 'u') {
            uint64_t value = length == NRT_DIAG_SIZE ? va_arg(args, size_t)
                             : length == NRT_DIAG_LLONG ? va_arg(args, unsigned long long)
                                                      : va_arg(args, unsigned int);
            size_t cursor = sizeof(digits);
            do {
                digits[--cursor] = (u8)('0' + value % 10);
                value /= 10;
            } while (value != 0);
            text = digits + cursor;
            count = sizeof(digits) - cursor;
        } else {
            // A new internal format must be implemented/tested, not silently
            // misread (which could consume the remaining arguments incorrectly).
            nrt_write_bytes(true, output.data, output.count);
            const char* invalid = "[invalid runtime diagnostic format]";
            nrt_write_bytes(true, invalid, nrt_cstr_count(invalid));
            return;
        }
        ++format;
        size_t padding = width > count ? width - count : 0;
        if (!left) {
            for (size_t i = 0; i < padding; ++i) nrt_diagnostic_byte(&output, ' ');
        }
        for (size_t i = 0; i < count; ++i) nrt_diagnostic_byte(&output, text[i]);
        if (left) {
            for (size_t i = 0; i < padding; ++i) nrt_diagnostic_byte(&output, ' ');
        }
    }
    nrt_write_bytes(true, output.data, output.count);
}
#endif

static void nrt_eprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
#if defined(NRT_LINUX_RAW_SYSCALLS)
    nrt_diagnostic_format(format, args);
#else
    vfprintf(stderr, format, args);
#endif
    va_end(args);
}

static void nrt_eprintfn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
#if defined(NRT_LINUX_RAW_SYSCALLS)
    nrt_diagnostic_format(format, args);
#else
    vfprintf(stderr, format, args);
#endif
    va_end(args);
    nrt_write_bytes(true, "\n", 1);
}

// Fatal runtime failures do not unwind Nerd or foreign atexit handlers. All
// Nerd output is already written; a failure in a worker must stop the process.
static _Noreturn void nrt_fatal_exit(void)
{
#if defined(NRT_LINUX_RAW_SYSCALLS)
    for (;;) nrt_linux_syscall6(NRT_SYS_EXIT_GROUP, 127, 0, 0, 0, 0, 0);
#else
    exit(127);
#endif
}

void nrt_pr(const void* data, size_t count)
{
    nrt_write_bytes(false, data, count);
}

void nrt_epr(const void* data, size_t count)
{
    nrt_write_bytes(true, data, count);
}

void nrt_prn(const void* data, size_t count)
{
    nrt_pr(data, count);
    nrt_write_bytes(false, "\n", 1);
}

void nrt_eprn(const void* data, size_t count)
{
    nrt_epr(data, count);
    nrt_write_bytes(true, "\n", 1);
}

void nrt_abort(const void* data, size_t count)
{
    nrt_eprn(data, count);
    nrt_fatal_exit();
}

void nrt_nerd_assert(bool condition,
                 const char* source_path,
                 uint32_t    line,
                 const NerdString* message)
{
    if (condition) {
        return;
    }
    nrt_eprintf("assertion failed at %s:%u: ", source_path, line);
    if (message != NULL && message->count > 0) {
        nrt_write_bytes(true, message->data, message->count);
    }
    nrt_eprintf("\n");
    nrt_fatal_exit();
}

bool nrt_string_eq(const NerdString* lhs, const NerdString* rhs)
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
    return nrt_bytes_compare(lhs->data, rhs->data, lhs->count) == 0;
}

bool nrt_slice_eq(const void* lhs,
              size_t      lhs_count,
              const void* rhs,
              size_t      rhs_count,
              size_t      element_size)
{
    if (lhs_count != rhs_count) {
        return false;
    }
    if (lhs_count == 0) {
        return true;
    }
    return nrt_bytes_compare(lhs, rhs, lhs_count * element_size) == 0;
}

// Bulk assignment checks remain enabled in release builds. Validate before
// forming element pointers or touching destination storage.
void nrt_slice_check(size_t count, size_t start, size_t end)
{
    if (start > end || end > count) {
        nrt_eprintfn("fatal: slice assignment range out of bounds");
        nrt_fatal_exit();
    }
}

void nrt_slice_copy(void*       destination,
                    size_t      count,
                    const void* source,
                    size_t      source_count,
                    size_t      element_size)
{
    if (count != source_count) {
        nrt_eprintfn("fatal: slice assignment lengths differ");
        nrt_fatal_exit();
    }
    if (element_size != 0 && count > SIZE_MAX / element_size) {
        nrt_eprintfn("fatal: slice assignment size overflow");
        nrt_fatal_exit();
    }
    if (count != 0 && element_size != 0) {
        nrt_bytes_move(destination, source, count * element_size);
    }
}

void nrt_slice_fill(void*       destination,
                    size_t      count,
                    const void* value,
                    size_t      element_size)
{
    if (element_size != 0 && count > SIZE_MAX / element_size) {
        nrt_eprintfn("fatal: slice assignment size overflow");
        nrt_fatal_exit();
    }
    if (element_size == 0) {
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        nrt_bytes_copy((u8*)destination + i * element_size, value, element_size);
    }
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
    nrt_fatal_exit();
}

static void* nrt_mem_reserve(size_t size, bool writable)
{
#if defined(_WIN32)
    DWORD protect = writable ? PAGE_READWRITE : PAGE_NOACCESS;
    DWORD type    = writable ? MEM_RESERVE | MEM_COMMIT : MEM_RESERVE;
    return VirtualAlloc(NULL, size, type, protect);
#elif defined(NRT_LINUX_RAW_SYSCALLS)
    int prot = writable ? PROT_READ | PROT_WRITE : PROT_NONE;
    intptr_t result = nrt_linux_syscall6(NRT_SYS_MMAP, 0, size, prot,
                                        MAP_PRIVATE | MAP_ANONYMOUS,
                                        (uintptr_t)-1, 0);
    // Linux reserves [-4095, -1] for syscall errors, including ENOMEM.
    return (uintptr_t)result >= (uintptr_t)-4095 ? NULL : (void*)result;
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
#elif defined(NRT_LINUX_RAW_SYSCALLS)
    return nrt_linux_syscall6(NRT_SYS_MPROTECT, (uintptr_t)memory, size,
                              PROT_READ | PROT_WRITE, 0, 0, 0) == 0;
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
#elif defined(NRT_LINUX_RAW_SYSCALLS)
    nrt_linux_syscall6(NRT_SYS_MUNMAP, (uintptr_t)memory, size, 0, 0, 0, 0);
#else
    munmap(memory, size);
#endif
}

static void nrt_heap_abort(const char* message)
{
    nrt_eprintfn("fatal: %s", message);
    nrt_fatal_exit();
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

// Live-list inspection requires quiescence: join workers before traversing.
void* nrt_mem_live_head(void)
{
#ifndef NDEBUG
    return g_nrt_heap_head;
#else
    return NULL;
#endif
}

void* nrt_arena_live_head(void)
{
#ifndef NDEBUG
    return g_nrt_arena_head;
#else
    return NULL;
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
    nrt_debug_lock();
    NrtHeapDebugHeader* debug = (NrtHeapDebugHeader*)mapping;
    debug->prev               = NULL;
    debug->next               = g_nrt_heap_head;
    debug->source_path        = (NerdString){
        .data  = (u8*)source_path,
        .count = source_path != NULL ? nrt_cstr_count(source_path) : 0,
    };
    debug->requested_size     = size;
    debug->index              = ++g_nrt_heap_next_index;
    debug->line               = line;
    if (debug->index == g_nrt_heap_break_index) {
        nrt_eprintfn("nrt: allocation break reached at index %llu",
                     (unsigned long long)debug->index);
    }
    if (g_nrt_heap_head != NULL) {
        g_nrt_heap_head->prev = debug;
    }
    g_nrt_heap_head = debug;
    nrt_debug_unlock();
#endif

    return user;
}

// Return a fresh header, including for an empty source. Elements are copied
// shallowly; references and nested dynamic-array handles retain their values.
void* nrt_array_copy(const void* source,
                     size_t      count,
                     size_t      item_size,
                     const char* source_path,
                     uint32_t    line)
{
    if (item_size != 0 && count > (SIZE_MAX - sizeof(NrtArray)) / item_size) {
        nrt_heap_abort("dynamic array copy size overflow");
    }
    size_t    bytes = count * item_size;
    NrtArray* result =
        nrt_mem_alloc(sizeof(NrtArray) + bytes, 16, source_path, line);
    *result = (NrtArray){.data = result + 1, .count = count, .capacity = count};
    if (bytes != 0) {
        nrt_bytes_copy(result->data, source, bytes);
    }
    return result;
}

// Validate before either backend reserves runtime-sized local stack storage.
size_t nrt_local_array_bytes(uint64_t count, size_t item_size)
{
    if (count > PTRDIFF_MAX ||
        (item_size != 0 && count > (uint64_t)PTRDIFF_MAX / item_size)) {
        nrt_heap_abort(
            "invalid runtime array length or allocation size overflow");
    }
    return (size_t)count * item_size;
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
        nrt_bytes_copy(result, memory, old_size < size ? old_size : size);
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
    nrt_debug_lock();
    nrt_heap_unlink(nrt_heap_debug_from_user(memory));
    nrt_debug_unlock();
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
    nrt_debug_lock();
    nrt_heap_unlink(nrt_heap_debug_from_user(memory));
    nrt_debug_unlock();
#else
    (void)memory;
#endif
}

void nrt_mem_break_on_alloc(uint64_t index)
{
#ifndef NDEBUG
    nrt_debug_lock();
    g_nrt_heap_break_index = index;
    nrt_debug_unlock();
#else
    (void)index;
#endif
}

#ifndef NDEBUG
static NrtArenaDebugNode* nrt_arena_debug_find(void* base)
{
    if (base == NULL) {
        return NULL;
    }

    for (NrtArenaDebugNode* node = g_nrt_arena_head; node != NULL;
         node                    = node->next) {
        if (node->base == base) {
            return node;
        }
    }
    return NULL;
}

static void nrt_arena_debug_unlink(NrtArenaDebugNode* node)
{
    if (node == NULL) {
        return;
    }

    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else if (g_nrt_arena_head == node) {
        g_nrt_arena_head = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
}

static void nrt_arena_debug_release(NrtArenaDebugNode* node)
{
    if (node == NULL) {
        return;
    }

    size_t mapping_size = node->mapping_size;
    nrt_arena_debug_unlink(node);
    nrt_mem_release(node, mapping_size);
}

static void nrt_arena_debug_track(NrtArena*   arena,
                                  const char* source_path,
                                  uint32_t    line)
{
    if (arena == NULL || arena->base == NULL || arena == nrt_temp_arena()) {
        return;
    }

    size_t mapping_size =
        nrt_align_up(sizeof(NrtArenaDebugNode), nrt_page_size());
    NrtArenaDebugNode* node =
        (NrtArenaDebugNode*)nrt_mem_reserve(mapping_size, true);
    if (node == NULL) {
        nrt_arena_abort("arena debug tracking allocation failed");
    }

    nrt_debug_lock();
    *node = (NrtArenaDebugNode){
        .prev           = NULL,
        .next           = g_nrt_arena_head,
        .base           = arena->base,
        .reserved_size  = NRT_ARENA_RESERVE_SIZE,
        .committed_size = arena->committed_size,
        .used_size      = (size_t)(arena->current - arena->base),
        .source_path    = (NerdString){
               .data  = (u8*)source_path,
               .count = source_path != NULL ? nrt_cstr_count(source_path) : 0,
        },
        .index          = ++g_nrt_arena_next_index,
        .line           = line,
        .mapping_size   = mapping_size,
    };
    if (g_nrt_arena_head != NULL) {
        g_nrt_arena_head->prev = node;
    }
    g_nrt_arena_head = node;
    nrt_debug_unlock();
}

static void nrt_arena_debug_update(NrtArena* arena)
{
    if (arena == NULL || arena->base == NULL) {
        return;
    }

    nrt_debug_lock();
    NrtArenaDebugNode* node = nrt_arena_debug_find(arena->base);
    if (node != NULL) {
        node->committed_size = arena->committed_size;
        node->used_size = (size_t)(arena->current - arena->base);
    }
    nrt_debug_unlock();
}

static void nrt_arena_debug_done(NrtArena* arena)
{
    if (arena == NULL || arena->base == NULL) {
        return;
    }
    nrt_debug_lock();
    nrt_arena_debug_release(nrt_arena_debug_find(arena->base));
    nrt_debug_unlock();
}
#endif

void nrt_arena_init(NrtArena*   arena,
                    size_t      initial_size,
                    size_t      increment,
                    const char* source_path,
                    uint32_t    line)
{
    (void)source_path;
    (void)line;
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
#ifndef NDEBUG
    nrt_arena_debug_track(arena, source_path, line);
#endif
}

void nrt_arena_done(NrtArena* arena)
{
    if (arena == NULL) {
        return;
    }
#ifndef NDEBUG
    nrt_arena_debug_done(arena);
#endif
    nrt_mem_release(arena->base, NRT_ARENA_RESERVE_SIZE);
    *arena = (NrtArena){0};
}

void nrt_arena_reset(NrtArena* arena)
{
    if (arena == NULL) {
        return;
    }
    arena->current = arena->base;
#ifndef NDEBUG
    nrt_arena_debug_update(arena);
#endif
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
#ifndef NDEBUG
    nrt_arena_debug_update(arena);
#endif
}

void* nrt_arena_alloc(NrtArena*   arena,
                      size_t      size,
                      size_t      alignment,
                      const char* source_path,
                      uint32_t    line)
{
    if (arena == NULL) {
        return NULL;
    }
    if (arena->base == NULL) {
        nrt_arena_init(arena, 0, 0, source_path, line);
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
#ifndef NDEBUG
    nrt_arena_debug_update(arena);
#endif
    return (void*)(arena->base + start);
}

void nrt_arena_pr(NerdString*       out,
                  NrtArena*         arena,
                  const NerdString* text)
{
    if (out == NULL) {
        return;
    }

    size_t count = text != NULL ? text->count : 0;
    uintptr_t begin = arena != NULL ? (uintptr_t)arena->base : 0;
    uintptr_t end = arena != NULL ? (uintptr_t)arena->current : 0;
    uintptr_t source = text != NULL ? (uintptr_t)text->data : 0;
    if (arena != NULL && arena->base != NULL && source >= begin &&
        source <= end && count <= end - source) {
        *out = *text;
        return;
    }
    u8* data = (u8*)nrt_arena_alloc(arena, count, 1, NULL, 0);
    if (count > 0) {
        nrt_bytes_copy(data, text->data, count);
    }
    *out = (NerdString){.data = data, .count = count};
}

void nrt_arena_prn(NerdString*       out,
                   NrtArena*         arena,
                   const NerdString* text)
{
    if (out == NULL) {
        return;
    }

    size_t count = text != NULL ? text->count : 0;
    if (count == SIZE_MAX) {
        nrt_arena_abort("arena string size overflow");
    }
    uintptr_t begin = arena != NULL ? (uintptr_t)arena->base : 0;
    uintptr_t end = arena != NULL ? (uintptr_t)arena->current : 0;
    uintptr_t source = text != NULL ? (uintptr_t)text->data : 0;
    if (arena != NULL && arena->base != NULL && source >= begin &&
        source <= end && count == end - source) {
        u8* newline = (u8*)nrt_arena_alloc(arena, 1, 1, NULL, 0);
        newline[0] = '\n';
        *out = (NerdString){.data = text->data, .count = count + 1};
        return;
    }
    u8* data = (u8*)nrt_arena_alloc(arena, count + 1, 1, NULL, 0);
    if (count > 0) {
        nrt_bytes_copy(data, text->data, count);
    }
    data[count] = '\n';
    *out = (NerdString){.data = data, .count = count + 1};
}

void nrt_temp_arena_reset(void) { nrt_string_builder_reset(); }

static bool string_is_utf8_boundary(const NerdString* value, size_t index)
{
    if (value == NULL || index > value->count) {
        return false;
    }
    return index == value->count || (value->data[index] & 0xC0) != 0x80;
}

void nrt_string_slice(NerdString* out,
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

static bool nrt_stderr_supports_colour(void)
{
#if defined(_WIN32)
    char no_colour[2];
    char term[5];
    if (GetEnvironmentVariableA("NO_COLOR", no_colour, sizeof(no_colour)) > 0 ||
        (GetEnvironmentVariableA("TERM", term, sizeof(term)) == 4 &&
         nrt_cstr_equal(term, "dumb"))) return false;
#else
    const char* no_colour = getenv("NO_COLOR");
    const char* term = getenv("TERM");
    if ((no_colour != NULL && no_colour[0] != '\0') ||
        (term != NULL && nrt_cstr_equal(term, "dumb"))) return false;
#endif
#if defined(_WIN32)
    HANDLE handle = nrt_windows_stream_handle(stderr);
    DWORD  mode   = 0;
    return handle != INVALID_HANDLE_VALUE && GetConsoleMode(handle, &mode) &&
           SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#elif defined(NRT_LINUX_RAW_SYSCALLS)
    u8 attributes[64]; // Large enough for the kernel TCGETS termios on both ABIs.
    return nrt_linux_syscall6(NRT_SYS_IOCTL, 2, 0x5401,
                              (uintptr_t)attributes, 0, 0, 0) == 0;
#else
    return isatty(STDERR_FILENO) != 0;
#endif
}

void nrt_report_main_error(const NerdString* message)
{
#if !defined(NRT_LINUX_RAW_SYSCALLS)
    fflush(stdout);
#endif
    bool colour = nrt_stderr_supports_colour();
    if (colour) nrt_epr("\033[31m", 5);
    nrt_epr("Error: ", 7);
    if (message != NULL) nrt_epr(message->data, message->count);
    if (colour) nrt_epr("\033[0m", 4);
    nrt_eprn(NULL, 0);
#if !defined(NRT_LINUX_RAW_SYSCALLS)
    fflush(stderr);
#endif
}

#ifndef NDEBUG
static size_t nrt_decimal_width(uint64_t value)
{
    size_t width = 1;
    while (value >= 10) {
        value /= 10;
        width++;
    }
    return width;
}

static void nrt_print_source_location(NerdString source_path, uint32_t line)
{
    if (source_path.count > 0) {
        nrt_write_bytes(true, source_path.data, source_path.count);
    } else {
        nrt_eprintf("<unknown>");
    }
    nrt_eprintf(":%u", line);
}

static void nrt_print_memory_leaks(void)
{
    if (g_nrt_heap_head == NULL && g_nrt_arena_head == NULL) {
        return;
    }

    size_t heap_count      = 0;
    size_t heap_bytes      = 0;
    size_t arena_count     = 0;
    size_t arena_committed = 0;
    size_t index_width     = nrt_cstr_count("index");
    size_t bytes_width     = nrt_cstr_count("bytes");
    for (NrtHeapDebugHeader* allocation = g_nrt_heap_head;
         allocation != NULL;
         allocation = allocation->next) {
        size_t allocation_index_width = nrt_decimal_width(allocation->index);
        size_t allocation_bytes_width =
            nrt_decimal_width(allocation->requested_size);
        if (allocation_index_width > index_width) {
            index_width = allocation_index_width;
        }
        if (allocation_bytes_width > bytes_width) {
            bytes_width = allocation_bytes_width;
        }
        heap_count++;
        heap_bytes += allocation->requested_size;
    }
    for (NrtArenaDebugNode* allocation = g_nrt_arena_head;
         allocation != NULL;
         allocation = allocation->next) {
        size_t allocation_index_width = nrt_decimal_width(allocation->index);
        size_t allocation_bytes_width =
            nrt_decimal_width(allocation->used_size);
        if (allocation_index_width > index_width) {
            index_width = allocation_index_width;
        }
        if (allocation_bytes_width > bytes_width) {
            bytes_width = allocation_bytes_width;
        }
        arena_count++;
        arena_committed += allocation->committed_size;
    }

    bool        colour = nrt_stderr_supports_colour();
    const char* reset  = colour ? "\x1b[0m" : "";
    const char* bold   = colour ? "\x1b[1m" : "";
    const char* red    = colour ? "\x1b[31m" : "";
    const char* yellow = colour ? "\x1b[33m" : "";
    const char* cyan   = colour ? "\x1b[36m" : "";

    nrt_eprintfn("%s%snrt: memory leaks detected%s", bold, red, reset);
    nrt_eprintfn("  %-5s %*s %*s  %s",
                 "type",
                 (int)index_width,
                 "index",
                 (int)bytes_width,
                 "bytes",
                 "location");
    for (NrtHeapDebugHeader* allocation = g_nrt_heap_head;
         allocation != NULL;
         allocation = allocation->next) {
        nrt_eprintf("  %s%-5s%s %*llu %*zu  %s",
                    yellow,
                    "heap",
                    reset,
                    (int)index_width,
                    (unsigned long long)allocation->index,
                    (int)bytes_width,
                    allocation->requested_size,
                    cyan);
        nrt_print_source_location(allocation->source_path, allocation->line);
        nrt_eprintfn("%s", reset);
    }
    for (NrtArenaDebugNode* allocation = g_nrt_arena_head;
         allocation != NULL;
         allocation = allocation->next) {
        nrt_eprintf("  %s%-5s%s %*llu %*zu  %s",
                    yellow,
                    "arena",
                    reset,
                    (int)index_width,
                    (unsigned long long)allocation->index,
                    (int)bytes_width,
                    allocation->used_size,
                    cyan);
        nrt_print_source_location(allocation->source_path, allocation->line);
        nrt_eprintfn("%s (%zu bytes committed)",
                     reset,
                     allocation->committed_size);
    }
    nrt_eprintfn("%snrt: total %zu heap leaks, %zu bytes; "
                 "%zu arena leaks, %zu bytes committed%s",
                 bold,
                 heap_count,
                 heap_bytes,
                 arena_count,
                 arena_committed,
                 reset);
}
#endif

void nrt_thread_init(void) {}

// Releases only the calling worker's lazy TLS storage; never reports process leaks.
void nrt_thread_done(void)
{
    if (g_string_builder_data != NULL) {
        nrt_mem_free(g_string_builder_data);
        g_string_builder_data     = NULL;
        g_string_builder_capacity = 0;
        g_string_builder_cursor   = 0;
    }
    nrt_arena_done(&g_temp_arena);
}

void nrt_core_init(void) { nrt_thread_init(); }

// Applications must join workers before process shutdown and live-list inspection.
void nrt_core_done(void)
{
    nrt_thread_done();
#ifndef NDEBUG
    nrt_debug_lock();
    nrt_print_memory_leaks();
    nrt_debug_unlock();
#endif
}

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

void nrt_string_builder_reset(void)
{
    g_string_builder_cursor = 0;
    nrt_arena_reset(&g_temp_arena);
}

size_t nrt_string_builder_mark(void) { return g_string_builder_cursor; }

void nrt_string_builder_append_string(const NerdString* str)
{
    if (str == NULL) {
        return;
    }
    string_builder_ensure_capacity(g_string_builder_cursor + str->count);

    if (str->count > 0) {
        nrt_bytes_copy(g_string_builder_data + g_string_builder_cursor,
               str->data,
               str->count);
        g_string_builder_cursor += str->count;
    }
}

void nrt_string_builder_append_byte(u8 byte)
{
    string_builder_ensure_capacity(g_string_builder_cursor + 1);

    g_string_builder_data[g_string_builder_cursor++] = byte;
}

void nrt_string_builder_finish_in(NerdString* out, size_t start, NrtArena* arena)
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
        data = (u8*)nrt_arena_alloc(arena, count, 1, NULL, 0);
        nrt_bytes_copy(data, g_string_builder_data + start, count);
    }
    *out = (NerdString){.data = data, .count = count};
    g_string_builder_cursor = start;
}

// Integer interpolation formats share one ABI across integer widths/backends.
void nrt_string_builder_append_integer(uint64_t value,
                                       uint32_t is_signed,
                                       uint32_t format)
{
    uint32_t    kind   = format & 255;
    uint32_t    base   = kind == 'b'                    ? 2
                         : kind == 'o'                  ? 8
                         : (kind == 'x' || kind == 'X') ? 16
                                                        : 10;
    const char* digits = kind == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
    bool        negative = is_signed && (int64_t)value < 0;
    if (negative) {
        value = (uint64_t)0 - value;
    }
    u8       reversed[64];
    uint32_t count = 0;
    do {
        reversed[count++] = (u8)digits[value % base];
        value /= base;
    } while (value);
    uint32_t width   = (format >> 8) & 2047;
    uint32_t length  = count + (negative ? 1 : 0);
    uint32_t padding = width > length ? width - length : 0;
    bool     zero    = (format & (1u << 19)) != 0;
    if (!zero) {
        for (uint32_t i = 0; i < padding; ++i) {
            nrt_string_builder_append_byte(' ');
        }
    }
    if (negative) {
        nrt_string_builder_append_byte('-');
    }
    if (zero) {
        for (uint32_t i = 0; i < padding; ++i) {
            nrt_string_builder_append_byte('0');
        }
    }
    while (count) {
        nrt_string_builder_append_byte(reversed[--count]);
    }
}

void nrt_string_builder_finish(NerdString* out, size_t start)
{
    nrt_string_builder_finish_in(out, start, &g_temp_arena);
}

void nrt_to_string_string(NerdString* out, const NerdString* value)
{
    if (out != NULL && value != NULL) {
        *out = *value;
    }
}

void nrt_to_string_bool(NerdString* out, bool value)
{
    if (out == NULL) {
        return;
    }
    *out = value ? (NerdString){.data = (u8*)"yes", .count = 3}
                 : (NerdString){.data = (u8*)"no", .count = 2};
}

static void nrt_integer_string(NerdString* out, u8* buffer,
                                uint64_t value, bool negative)
{
    if (out == NULL) return;
    // Unsigned subtraction also handles INT64_MIN without signed overflow.
    if (negative) value = (uint64_t)0 - value;
    u8 reversed[20];
    size_t count = 0;
    do {
        reversed[count++] = (u8)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    size_t cursor = 0;
    if (negative) buffer[cursor++] = '-';
    while (count != 0) buffer[cursor++] = reversed[--count];
    buffer[cursor] = '\0';
    *out = (NerdString){.data = buffer, .count = cursor};
}

#define DEF_TO_STRING_INT(name, type, signed_value)                            \
    void nrt_to_string_##name(NerdString* out, type value)                      \
    {                                                                         \
        static _Thread_local u8 buffer[21];                                    \
        nrt_integer_string(out, buffer, (uint64_t)value,                        \
                           (signed_value) && (int64_t)value < 0);              \
    }

#define DEF_TO_STRING_FLOAT(name, type)                                        \
    void nrt_to_string_##name(NerdString* out, type value)                         \
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

DEF_TO_STRING_INT(i8, int8_t, true)
DEF_TO_STRING_INT(i16, int16_t, true)
DEF_TO_STRING_INT(i32, int, true)
DEF_TO_STRING_INT(i64, int64_t, true)
DEF_TO_STRING_INT(u8, uint8_t, false)
DEF_TO_STRING_INT(u16, uint16_t, false)
DEF_TO_STRING_INT(u32, uint32_t, false)
DEF_TO_STRING_INT(u64, uint64_t, false)
DEF_TO_STRING_INT(isize, intptr_t, true)
DEF_TO_STRING_INT(usize, uintptr_t, false)
DEF_TO_STRING_FLOAT(f32, float)
DEF_TO_STRING_FLOAT(f64, double)

#if defined(NRT_LINUX_RAW_SYSCALLS)
static int g_nrt_stdin_pushback = -1;
static atomic_flag g_nrt_stdin_lock = ATOMIC_FLAG_INIT;
#endif

static int nrt_stdin_byte(void)
{
#if defined(NRT_LINUX_RAW_SYSCALLS)
    if (g_nrt_stdin_pushback >= 0) {
        int byte = g_nrt_stdin_pushback;
        g_nrt_stdin_pushback = -1;
        return byte;
    }
    u8 byte;
    intptr_t result;
    do {
        result = nrt_linux_syscall6(NRT_SYS_READ, 0, (uintptr_t)&byte, 1, 0, 0, 0);
    } while (result == -4); // EINTR; EOF/other errors terminate this read.
    return result == 1 ? byte : -1;
#else
    return getchar();
#endif
}

static void nrt_stdin_unread(int byte)
{
#if defined(NRT_LINUX_RAW_SYSCALLS)
    g_nrt_stdin_pushback = byte;
#else
    ungetc(byte, stdin);
#endif
}

size_t nrt_read_line(u8* buffer, size_t capacity)
{
#if defined(NRT_LINUX_RAW_SYSCALLS)
    // The descriptor and its single-byte pushback belong to the process, not TLS.
    while (atomic_flag_test_and_set_explicit(&g_nrt_stdin_lock, memory_order_acquire)) {}
#endif
    size_t count = 0;
    for (;;) {
        int byte = nrt_stdin_byte();
        if (byte < 0 || byte == '\n') break;
        if (byte == '\r') {
            int next = nrt_stdin_byte();
            if (next >= 0 && next != '\n') nrt_stdin_unread(next);
            break;
        }
        // Consume the whole line even if the caller supplied a short/empty view.
        if (count < capacity) buffer[count++] = (u8)byte;
    }
#if defined(NRT_LINUX_RAW_SYSCALLS)
    atomic_flag_clear_explicit(&g_nrt_stdin_lock, memory_order_release);
#endif
    return count;
}

// C owns va_list's representation and traversal. The receiving LLVM function
// invokes llvm.va_start on the first member; borrowed cursors never own it.
typedef struct NrtVaList {
    va_list           args;
    struct NrtVaList* owner;
    struct NrtVaList* next;
} NrtVaList;

NrtVaList* nrt_va_create(void)
{
    NrtVaList* cursor = nrt_mem_alloc(sizeof(*cursor), _Alignof(NrtVaList), NULL, 0);
    if (cursor == NULL) {
        abort();
    }
    *cursor = (NrtVaList){0};
    cursor->owner = cursor;
    return cursor;
}

NrtVaList* nrt_va_copy(NrtVaList* source)
{
    NrtVaList* cursor = nrt_va_create();
    va_copy(cursor->args, source->args);
    cursor->owner       = source->owner;
    cursor->next        = cursor->owner->next;
    cursor->owner->next = cursor;
    return cursor;
}

void nrt_va_done(NrtVaList* cursor)
{
    while (cursor != NULL) {
        NrtVaList* next = cursor->next;
        va_end(cursor->args);
        nrt_mem_free(cursor);
        cursor = next;
    }
}

#define NRT_VA_NEXT(name, type)                                                \
    type nrt_va_next_##name(NrtVaList* cursor)                                 \
    {                                                                          \
        return va_arg(cursor->args, type);                                     \
    }
NRT_VA_NEXT(i32, int32_t)
NRT_VA_NEXT(u32, uint32_t)
NRT_VA_NEXT(i64, int64_t)
NRT_VA_NEXT(u64, uint64_t)
NRT_VA_NEXT(isize, intptr_t)
NRT_VA_NEXT(usize, uintptr_t)
NRT_VA_NEXT(f64, double)
NRT_VA_NEXT(ptr, void*)
#undef NRT_VA_NEXT

void nrt_va_format(NerdString* out, NrtVaList* cursor, const char* format)
{
    va_list measure;
    va_copy(measure, cursor->args);
    int count = vsnprintf(NULL, 0, format, measure);
    va_end(measure);
    if (count < 0) {
        *out = (NerdString){0};
        return;
    }
    char* buffer =
        nrt_arena_alloc(nrt_temp_arena(), (size_t)count + 1, 1, NULL, 0);
    va_list render;
    va_copy(render, cursor->args);
    int written = vsnprintf(buffer, (size_t)count + 1, format, render);
    va_end(render);
    *out = written < 0
               ? (NerdString){0}
               : (NerdString){(u8*)buffer,
                              (size_t)written < (size_t)count ? (size_t)written
                                                              : (size_t)count};
}

// The C parameter adjustment differs even between the two x86-64 ABIs.
void* nrt_va_native(NrtVaList* cursor)
{
#if defined(_WIN32)
    return cursor->args;
#else
    return &cursor->args;
#endif
}
