// Linux executable substrate. Hosted/shared runtimes deliberately exclude this.
#ifndef NRT_LINUX_INCLUDED
#define NRT_LINUX_INCLUDED
#if defined(NRT_FREESTANDING)
#    include <elf.h>
#    include <sys/select.h>
#    ifndef NRT_MATH_INCLUDED
#        include "nmath.h"
#    endif
#    pragma GCC visibility push(hidden)
static _Thread_local volatile uintptr_t nrt_tls_anchor = 1;
static _Thread_local int                nrt_errno;
#    if defined(__aarch64__)
uintptr_t __stack_chk_guard;
#    endif
static const u8* nrt_tls_image;
static size_t    nrt_tls_files, nrt_tls_memory, nrt_tls_alignment;
static uintptr_t nrt_canary;
static char**    nrt_environment;

static NRT_NO_BUILTIN void* nrt_linux_tls(void)
{
    size_t alignment = nrt_tls_alignment;
    if (alignment > SIZE_MAX / 4 || nrt_tls_memory > SIZE_MAX / 2) {
        return NULL;
    }
    size_t size = (nrt_tls_memory + alignment - 1) & ~(alignment - 1);
    if (size >
        SIZE_MAX - 2 * alignment - 128 - g_nrt_process_metadata.page_size) {
        return NULL;
    }
    size_t map_size =
        size + 2 * alignment + 128 + g_nrt_process_metadata.page_size;
    intptr_t raw = nrt_linux_syscall6(
        NRT_SYS_MMAP, 0, map_size, 3, 0x22, (uintptr_t)-1, 0);
    if ((uintptr_t)raw >= (uintptr_t)-4095) {
        return NULL;
    }
    u8* base = (u8*)(((uintptr_t)raw + alignment - 1) & ~(alignment - 1));
#    if defined(__x86_64__)
    u8* thread_pointer = base + size;
    for (size_t i = 0; i < nrt_tls_files; ++i) {
        base[i] = nrt_tls_image[i];
    }
    *(uintptr_t*)thread_pointer        = (uintptr_t)thread_pointer;
    *(uintptr_t*)(thread_pointer + 8)  = (uintptr_t)raw;
    *(uintptr_t*)(thread_pointer + 16) = map_size;
    *(uintptr_t*)(thread_pointer + 40) = nrt_canary;
#    else
    u8* thread_pointer                = base;
    *(uintptr_t*)thread_pointer       = (uintptr_t)raw;
    *(uintptr_t*)(thread_pointer + 8) = map_size;
    size_t offset                     = (16 + alignment - 1) & ~(alignment - 1);
    for (size_t i = 0; i < nrt_tls_files; ++i) {
        base[offset + i] = nrt_tls_image[i];
    }
#    endif
    return thread_pointer;
}

static NRT_NO_BUILTIN void nrt_linux_set_tls(void* pointer)
{
#    if defined(__x86_64__)
    if (nrt_linux_syscall6(158, 0x1002, (uintptr_t)pointer, 0, 0, 0, 0) != 0) {
        __builtin_trap();
    }
#    else
    __asm__ volatile("msr tpidr_el0, %0" ::"r"(pointer) : "memory");
    __stack_chk_guard = nrt_canary;
#    endif
}

typedef void (*NrtInitializer)(int, char**, char**);
extern NrtInitializer __preinit_array_start[], __preinit_array_end[];
extern NrtInitializer __init_array_start[], __init_array_end[];
extern void (*__fini_array_start[])(void), (*__fini_array_end[])(void);
extern int main(int, char**);
// Keep all TLS expressions behind a non-inlined call after thread-pointer
// installation. Optimisers may otherwise hoist a supposedly invariant TLS base
// read across the raw installation syscall/system-register write.
static __attribute__((noinline, noreturn)) void
nrt_linux_run(uintptr_t         argc,
              char**            argv,
              char**            environment,
              const Elf64_Phdr* headers,
              size_t            number)
{
    // Force an initialised TLS section even if every other TLS value is zero.
    nrt_tls_anchor = 1;
    // The kernel does not apply GNU RELRO. There are no dynamic relocations;
    // make the link-resolved tables read-only before invoking constructors.
    size_t page    = g_nrt_process_metadata.page_size;
    for (size_t i = 0; i < number; ++i) {
        if (headers[i].p_type == PT_GNU_RELRO) {
            uintptr_t begin = headers[i].p_vaddr & ~(uintptr_t)(page - 1);
            uintptr_t end   = (headers[i].p_vaddr + headers[i].p_memsz) &
                              ~(uintptr_t)(page - 1);
            if (end > begin &&
                nrt_linux_syscall6(
                    NRT_SYS_MPROTECT, begin, end - begin, 1, 0, 0, 0) != 0) {
                __builtin_trap();
            }
        }
    }
    for (NrtInitializer* p = __preinit_array_start; p != __preinit_array_end;
         ++p) {
        (*p)((int)argc, argv, environment);
    }
    for (NrtInitializer* p = __init_array_start; p != __init_array_end; ++p) {
        (*p)((int)argc, argv, environment);
    }
    int status = main((int)argc, argv);
    for (void (**p)(void) = __fini_array_end; p != __fini_array_start;) {
        (*--p)();
    }
    nrt_linux_syscall6(NRT_SYS_EXIT_GROUP, (uint32_t)status, 0, 0, 0, 0, 0);
    __builtin_trap();
}
__attribute__((
    used, visibility("hidden"), no_stack_protector, no_builtin, noreturn)) void
nrt_linux_start(uintptr_t* stack)
{
    uintptr_t argc        = *stack;
    char**    argv        = (char**)(stack + 1);
    char**    environment = argv + argc + 1;
    char**    end         = environment;
    while (*end) {
        ++end;
    }
    const uintptr_t* auxiliary = (const uintptr_t*)(end + 1);
    if (argc > INT_MAX ||
        !nrt_read_process_metadata((const char* const*)environment,
                                   auxiliary,
                                   &g_nrt_process_metadata)) {
        __builtin_trap();
    }
    nrt_environment           = environment;
    const Elf64_Phdr* headers = NULL;
    size_t            number = 0, entry_size = 0;
    const u8*         random = NULL;
    for (const uintptr_t* a = auxiliary; a[0]; a += 2) {
        if (a[0] == AT_PHDR) {
            headers = (const Elf64_Phdr*)a[1];
        }
        if (a[0] == AT_PHNUM) {
            number = a[1];
        }
        if (a[0] == AT_PHENT) {
            entry_size = a[1];
        }
        if (a[0] == AT_RANDOM) {
            random = (const u8*)a[1];
        }
    }
    if (!headers || entry_size != sizeof(*headers) || !random) {
        __builtin_trap();
    }
    for (size_t i = 0; i < sizeof(uintptr_t); ++i) {
        nrt_canary |= (uintptr_t)random[i] << (i * 8);
    }
    nrt_canary &= ~(uintptr_t)255;
    for (size_t i = 0; i < number; ++i) {
        if (headers[i].p_type == PT_TLS) {
            nrt_tls_image     = (const u8*)(uintptr_t)headers[i].p_vaddr;
            nrt_tls_files     = headers[i].p_filesz;
            nrt_tls_memory    = headers[i].p_memsz;
            nrt_tls_alignment = headers[i].p_align;
        }
    }
    if (!nrt_tls_alignment || (nrt_tls_alignment & (nrt_tls_alignment - 1)) ||
        nrt_tls_files > nrt_tls_memory) {
        __builtin_trap();
    }
    void* pointer = nrt_linux_tls();
    if (!pointer) {
        __builtin_trap();
    }
    nrt_linux_set_tls(pointer);
    nrt_linux_run(argc, argv, environment, headers, number);
}
#    if defined(__x86_64__)
__asm__(
    ".text\n.global _start\n.hidden _start\n.type _start,@function\n_start:\n"
    "xor %ebp,%ebp\nmov %rsp,%rdi\nand $-16,%rsp\ncall "
    "nrt_linux_start\nud2\n.size _start,.-_start\n");
#    else
__asm__(
    ".text\n.global _start\n.hidden _start\n.type _start,%function\n_start:\n"
    "mov x0,sp\nmov x29,xzr\nmov x30,xzr\nbl nrt_linux_start\nbrk #0\n.size "
    "_start,.-_start\n");
#    endif

_Noreturn void abort(void) { nrt_signal_abort(); }

// LLVM and C code generation may lower intrinsics to these compiler ABI names.
NRT_NO_BUILTIN void* memcpy(void* out, const void* in, size_t n)
{
    nrt_bytes_copy(out, in, n);
    return out;
}
NRT_NO_BUILTIN void* memmove(void* out, const void* in, size_t n)
{
    nrt_bytes_move(out, in, n);
    return out;
}
NRT_NO_BUILTIN void* memset(void* out, int byte, size_t n)
{
    u8* p = out;
    for (size_t i = 0; i < n; ++i) {
        p[i] = (u8)byte;
    }
    return out;
}
NRT_NO_BUILTIN int memcmp(const void* a, const void* b, size_t n)
{
    return nrt_bytes_compare(a, b, n);
}
NRT_NO_BUILTIN int bcmp(const void* a, const void* b, size_t n)
{
    return nrt_bytes_compare(a, b, n);
}
NRT_NO_BUILTIN size_t strlen(const char* s) { return nrt_cstr_count(s); }
NRT_NO_BUILTIN int    strcmp(const char* a, const char* b)
{
    while (*a && *a == *b) {
        ++a;
        ++b;
    }
    return (u8)*a - (u8)*b;
}
int*            __errno_location(void) { return &nrt_errno; }
static intptr_t nrt_os_result(intptr_t result)
{
    if ((uintptr_t)result >= (uintptr_t)-4095) {
        nrt_errno = (int)-result;
        return -1;
    }
    return result;
}
long syscall(long number, ...)
{
    va_list args;
    va_start(args, number);
    uintptr_t a[6];
    for (size_t i = 0; i < 6; ++i) {
        a[i] = va_arg(args, uintptr_t);
    }
    va_end(args);
    return nrt_os_result(
        nrt_linux_syscall6(number, a[0], a[1], a[2], a[3], a[4], a[5]));
}
#    if defined(__x86_64__)
enum {
    NS_SELECT   = 23,
    NS_SOCKET   = 41,
    NS_BIND     = 49,
    NS_LISTEN   = 50,
    NS_ACCEPT   = 43,
    NS_CONNECT  = 42,
    NS_LOCAL    = 51,
    NS_PEER     = 52,
    NS_SEND     = 44,
    NS_RECV     = 45,
    NS_SHUTDOWN = 48,
    NS_OPTION   = 55,
    NS_CLOSE    = 3,
    NS_IOCTL    = 16,
    NS_EXEC     = 59
};
#    else
enum {
    NS_SELECT   = 72,
    NS_SOCKET   = 198,
    NS_BIND     = 200,
    NS_LISTEN   = 201,
    NS_ACCEPT   = 202,
    NS_CONNECT  = 203,
    NS_LOCAL    = 204,
    NS_PEER     = 205,
    NS_SEND     = 206,
    NS_RECV     = 207,
    NS_SHUTDOWN = 210,
    NS_OPTION   = 209,
    NS_CLOSE    = 57,
    NS_IOCTL    = 29,
    NS_EXEC     = 221
};
#    endif
int socket(int family, int type, int protocol)
{
    return (int)nrt_os_result(
        nrt_linux_syscall6(NS_SOCKET, family, type, protocol, 0, 0, 0));
}
int bind(int fd, const void* address, unsigned length)
{
    return (int)nrt_os_result(
        nrt_linux_syscall6(NS_BIND, fd, (uintptr_t)address, length, 0, 0, 0));
}
int listen(int fd, int backlog)
{
    return (int)nrt_os_result(
        nrt_linux_syscall6(NS_LISTEN, fd, backlog, 0, 0, 0, 0));
}
int accept(int fd, void* address, unsigned* length)
{
    return (int)nrt_os_result(nrt_linux_syscall6(
        NS_ACCEPT, fd, (uintptr_t)address, (uintptr_t)length, 0, 0, 0));
}
int connect(int fd, const void* address, unsigned length)
{
    return (int)nrt_os_result(nrt_linux_syscall6(
        NS_CONNECT, fd, (uintptr_t)address, length, 0, 0, 0));
}
int getsockname(int fd, void* address, unsigned* length)
{
    return (int)nrt_os_result(nrt_linux_syscall6(
        NS_LOCAL, fd, (uintptr_t)address, (uintptr_t)length, 0, 0, 0));
}
int getpeername(int fd, void* address, unsigned* length)
{
    return (int)nrt_os_result(nrt_linux_syscall6(
        NS_PEER, fd, (uintptr_t)address, (uintptr_t)length, 0, 0, 0));
}
long sendto(int         fd,
            const void* data,
            size_t      count,
            int         flags,
            const void* address,
            unsigned    length)
{
    return nrt_os_result(nrt_linux_syscall6(NS_SEND,
                                            fd,
                                            (uintptr_t)data,
                                            count,
                                            flags,
                                            (uintptr_t)address,
                                            length));
}
long recvfrom(int       fd,
              void*     data,
              size_t    count,
              int       flags,
              void*     address,
              unsigned* length)
{
    return nrt_os_result(nrt_linux_syscall6(NS_RECV,
                                            fd,
                                            (uintptr_t)data,
                                            count,
                                            flags,
                                            (uintptr_t)address,
                                            (uintptr_t)length));
}
long send(int fd, const void* data, size_t count, int flags)
{
    return sendto(fd, data, count, flags, NULL, 0);
}
long recv(int fd, void* data, size_t count, int flags)
{
    return recvfrom(fd, data, count, flags, NULL, NULL);
}
int shutdown(int fd, int how)
{
    return (int)nrt_os_result(
        nrt_linux_syscall6(NS_SHUTDOWN, fd, how, 0, 0, 0, 0));
}
int getsockopt(int fd, int level, int option, void* value, unsigned* length)
{
    return (int)nrt_os_result(nrt_linux_syscall6(
        NS_OPTION, fd, level, option, (uintptr_t)value, (uintptr_t)length, 0));
}
int close(int fd)
{
    return (int)nrt_os_result(nrt_linux_syscall6(NS_CLOSE, fd, 0, 0, 0, 0, 0));
}
int ioctl(int fd, unsigned long request, ...)
{
    va_list a;
    va_start(a, request);
    void* p = va_arg(a, void*);
    va_end(a);
    return (int)nrt_os_result(
        nrt_linux_syscall6(NS_IOCTL, fd, request, (uintptr_t)p, 0, 0, 0));
}
uint16_t htons(uint16_t x) { return __builtin_bswap16(x); }
uint16_t ntohs(uint16_t x) { return __builtin_bswap16(x); }
uint32_t htonl(uint32_t x) { return __builtin_bswap32(x); }
uint32_t ntohl(uint32_t x) { return __builtin_bswap32(x); }
int      select(int             count,
                fd_set*         read_set,
                fd_set*         write_set,
                fd_set*         exceptional,
                struct timeval* timeout)
{
#    if defined(__x86_64__)
    return (int)nrt_os_result(nrt_linux_syscall6(NS_SELECT,
                                                 count,
                                                 (uintptr_t)read_set,
                                                 (uintptr_t)write_set,
                                                 (uintptr_t)exceptional,
                                                 (uintptr_t)timeout,
                                                 0));
#    else
    long* tv = (long*)timeout;
    long  ts[2];
    if (tv) {
        ts[0] = tv[0];
        ts[1] = tv[1] * 1000;
    }
    intptr_t result = nrt_linux_syscall6(NS_SELECT,
                                         count,
                                         (uintptr_t)read_set,
                                         (uintptr_t)write_set,
                                         (uintptr_t)exceptional,
                                         tv ? (uintptr_t)ts : 0,
                                         0);
    if (tv) {
        tv[0] = ts[0];
        tv[1] = ts[1] / 1000;
    }
    return (int)nrt_os_result(result);
#    endif
}
static int nrt_exec_file(const char* file, char* const* arguments)
{
    intptr_t result = nrt_linux_syscall6(NS_EXEC,
                                         (uintptr_t)file,
                                         (uintptr_t)arguments,
                                         (uintptr_t)nrt_environment,
                                         0,
                                         0,
                                         0);
    if (result == -8) { // execvp's ENOEXEC shell-script compatibility.
        size_t count = 0;
        while (arguments[count]) {
            ++count;
        }
        size_t   bytes = (count ? count + 2 : 3) * sizeof(char*);
        intptr_t raw   = nrt_linux_syscall6(
            NRT_SYS_MMAP, 0, bytes, 3, 0x22, (uintptr_t)-1, 0);
        if ((uintptr_t)raw >= (uintptr_t)-4095) {
            return (int)nrt_os_result(raw);
        }
        char** shell = (char**)raw;
        shell[0]     = "/bin/sh";
        shell[1]     = (char*)file;
        for (size_t i = 1; i < count; ++i) {
            shell[i + 1] = arguments[i];
        }
        shell[count ? count + 1 : 2] = NULL;
        result = nrt_linux_syscall6(NS_EXEC,
                                    (uintptr_t)shell[0],
                                    (uintptr_t)shell,
                                    (uintptr_t)nrt_environment,
                                    0,
                                    0,
                                    0);
        nrt_linux_syscall6(NRT_SYS_MUNMAP, raw, bytes, 0, 0, 0, 0);
    }
    return (int)nrt_os_result(result);
}
int execvp(const char* file, char* const* arguments)
{
    if (!*file) {
        nrt_errno = 2;
        return -1;
    }
    for (const char* p = file; *p; ++p) {
        if (*p == '/') {
            return nrt_exec_file(file, arguments);
        }
    }
    const char* path = "/bin:/usr/bin";
    for (char** e = nrt_environment; *e; ++e) {
        const char* v = nrt_environment_value(*e, "PATH");
        if (v) {
            path = v;
            break;
        }
    }
    size_t file_size = nrt_cstr_count(file);
    bool   denied    = false;
    if (file_size >= 4095) {
        nrt_errno = 36;
        return -1;
    }
    for (;;) {
        const char* end = path;
        while (*end && *end != ':') {
            ++end;
        }
        size_t prefix = (size_t)(end - path);
        char candidate[4096]; // Linux PATH_MAX, not a virtual-memory page size.
        if (prefix + file_size + 2 <= sizeof(candidate)) {
            nrt_bytes_copy(candidate, path, prefix);
            size_t offset = prefix;
            if (prefix) {
                candidate[offset++] = '/';
            }
            nrt_bytes_copy(candidate + offset, file, file_size + 1);
            nrt_exec_file(candidate, arguments);
            if (nrt_errno == 13) {
                denied = true;
            } else if (nrt_errno != 2 && nrt_errno != 20 && nrt_errno != 116 &&
                       nrt_errno != 19 && nrt_errno != 110) {
                return -1;
            }
        }
        if (!*end) {
            break;
        }
        path = end + 1;
    }
    if (denied) {
        nrt_errno = 13;
    } else {
        nrt_errno = 2;
    }
    return -1;
}

// Joinable kernel threads and private futex synchronisation. The public
// bindings retain their opaque storage/entry ABI; this is not a glibc TCB.
typedef struct {
    _Atomic int tid;
    void*       stack;
    size_t      stack_size;
    void*       tls;
    void* (*entry)(void*);
    void* context;
    void* result;
} NrtNativeThread;
static _Thread_local NrtNativeThread* nrt_native_self;
static intptr_t nrt_futex(_Atomic unsigned* word, int operation, unsigned value)
{
#    if defined(__x86_64__)
    const int number = 202;
#    else
    const int number = 98;
#    endif
    return nrt_linux_syscall6(
        number, (uintptr_t)word, operation, value, 0, 0, 0);
}
__attribute__((used, visibility("hidden"), noreturn)) void
nrt_native_thread_entry(NrtNativeThread* thread)
{
    nrt_native_self = thread;
    thread->result  = thread->entry(thread->context);
#    if defined(__x86_64__)
    const int number = 60;
#    else
    const int number = 93;
#    endif
    nrt_linux_syscall6(number, 0, 0, 0, 0, 0, 0);
    __builtin_trap();
}
extern long nrt_native_clone(void*, NrtNativeThread*, _Atomic int*, void*);
#    if defined(__x_64__) || defined(__x86_64__)
__asm__(".text\n.global nrt_native_clone\n.hidden nrt_native_clone\n.type "
        "nrt_native_clone,@function\nnrt_native_clone:\n"
        "mov %rsi,-16(%rdi)\nlea -16(%rdi),%rsi\nmov %rdx,%r10\nmov "
        "%rcx,%r8\nmov $0x3d0f00,%edi\nmov $56,%eax\nsyscall\n"
        "test %rax,%rax\njnz 1f\nmov (%rsp),%rdi\ncall "
        "nrt_native_thread_entry\nud2\n1: ret\n.size "
        "nrt_native_clone,.-nrt_native_clone\n");
#    else
__asm__(".text\n.global nrt_native_clone\n.hidden nrt_native_clone\n.type "
        "nrt_native_clone,%function\nnrt_native_clone:\n"
        "sub x9,x0,#16\nstr x1,[x9]\nmov x4,x2\nmov x1,x9\nmov x0,#0xf00\nmovk "
        "x0,#0x3d,lsl #16\nmov x8,#220\nsvc #0\n"
        "cbnz x0,1f\nldr x0,[sp]\nbl nrt_native_thread_entry\nbrk #0\n1: "
        "ret\n.size nrt_native_clone,.-nrt_native_clone\n");
#    endif
static void nrt_tls_free(void* pointer)
{
#    if defined(__x86_64__)
    uintptr_t* allocation = (uintptr_t*)((u8*)pointer + 8);
#    else
    uintptr_t* allocation = pointer;
#    endif
    nrt_linux_syscall6(
        NRT_SYS_MUNMAP, allocation[0], allocation[1], 0, 0, 0, 0);
}
int pthread_create(uintptr_t*  handle,
                   const void* attributes,
                   void* (*entry)(void*),
                   void* context)
{
    if (attributes || !handle || !entry) {
        return 22;
    }
    size_t page   = g_nrt_process_metadata.page_size;
    size_t usable = 8 * 1024 * 1024;
    usable        = (usable + page - 1) & ~(page - 1);
    size_t   size = usable + 2 * page;
    intptr_t memory =
        nrt_linux_syscall6(NRT_SYS_MMAP, 0, size, 0, 0x22, (uintptr_t)-1, 0);
    if ((uintptr_t)memory >= (uintptr_t)-4095) {
        return (int)-memory;
    }
    if (nrt_linux_syscall6(
            NRT_SYS_MPROTECT, memory + page, usable, 3, 0, 0, 0) != 0) {
        nrt_linux_syscall6(NRT_SYS_MUNMAP, memory, size, 0, 0, 0, 0);
        return 12;
    }
    void* tls = nrt_linux_tls();
    if (!tls) {
        nrt_linux_syscall6(NRT_SYS_MUNMAP, memory, size, 0, 0, 0, 0);
        return 12;
    }
    intptr_t control = nrt_linux_syscall6(
        NRT_SYS_MMAP, 0, sizeof(NrtNativeThread), 3, 0x22, (uintptr_t)-1, 0);
    if ((uintptr_t)control >= (uintptr_t)-4095) {
        nrt_tls_free(tls);
        nrt_linux_syscall6(NRT_SYS_MUNMAP, memory, size, 0, 0, 0, 0);
        return (int)-control;
    }
    NrtNativeThread* thread = (NrtNativeThread*)control;
    atomic_init(&thread->tid, 0);
    thread->stack      = (void*)memory;
    thread->stack_size = size;
    thread->tls        = tls;
    thread->entry      = entry;
    thread->context    = context;
    thread->result     = NULL;
    long result =
        nrt_native_clone((u8*)memory + size - page, thread, &thread->tid, tls);
    if (result < 0) {
        nrt_tls_free(tls);
        nrt_linux_syscall6(NRT_SYS_MUNMAP, memory, size, 0, 0, 0, 0);
        nrt_linux_syscall6(
            NRT_SYS_MUNMAP, (uintptr_t)thread, sizeof(*thread), 0, 0, 0, 0);
        return (int)-result;
    }
    *handle = (uintptr_t)thread;
    return 0;
}
uintptr_t pthread_self(void)
{
    return nrt_native_self ? (uintptr_t)nrt_native_self
                           : (uintptr_t)&nrt_tls_anchor;
}
int pthread_equal(uintptr_t a, uintptr_t b) { return a == b; }
int pthread_join(uintptr_t handle, void** result)
{
    if (handle == pthread_self()) {
        return 35;
    }
    if (!handle) {
        return 3;
    }
    NrtNativeThread* thread = (NrtNativeThread*)handle;
    for (;;) {
        int tid = atomic_load_explicit(&thread->tid, memory_order_acquire);
        if (!tid) {
            break;
        }
        nrt_futex((_Atomic unsigned*)&thread->tid, 0, (unsigned)tid);
    }
    if (result) {
        *result = thread->result;
    }
    nrt_tls_free(thread->tls);
    nrt_linux_syscall6(NRT_SYS_MUNMAP,
                       (uintptr_t)thread->stack,
                       thread->stack_size,
                       0,
                       0,
                       0,
                       0);
    nrt_linux_syscall6(
        NRT_SYS_MUNMAP, (uintptr_t)thread, sizeof(*thread), 0, 0, 0, 0);
    return 0;
}
int pthread_mutex_init(void* mutex, const void* attributes)
{
    if (attributes) {
        return 22;
    }
    atomic_init((_Atomic unsigned*)mutex, 0);
    return 0;
}
int pthread_mutex_destroy(void* mutex)
{
    return atomic_load((_Atomic unsigned*)mutex) ? 16 : 0;
}
int pthread_mutex_lock(void* mutex)
{
    _Atomic unsigned* state    = mutex;
    unsigned          expected = 0;
    if (atomic_compare_exchange_strong_explicit(
            state, &expected, 1, memory_order_acquire, memory_order_relaxed)) {
        return 0;
    }
    while (atomic_exchange_explicit(state, 2, memory_order_acquire) != 0) {
        nrt_futex(state, 128, 2);
    }
    return 0;
}
int pthread_mutex_unlock(void* mutex)
{
    if (atomic_exchange_explicit(
            (_Atomic unsigned*)mutex, 0, memory_order_release) == 2) {
        nrt_futex(mutex, 129, 1);
    }
    return 0;
}
int pthread_cond_init(void* condition, const void* attributes)
{
    if (attributes) {
        return 22;
    }
    _Atomic unsigned* p = condition;
    atomic_init(p, 0);
    atomic_init(p + 1, 0);
    return 0;
}
int pthread_cond_destroy(void* condition)
{
    return atomic_load((_Atomic unsigned*)condition + 1) ? 16 : 0;
}
int pthread_cond_wait(void* condition, void* mutex)
{
    _Atomic unsigned* sequence = condition;
    unsigned previous = atomic_load_explicit(sequence, memory_order_acquire);
    atomic_fetch_add(sequence + 1, 1);
    pthread_mutex_unlock(mutex);
    nrt_futex(sequence, 128, previous);
    atomic_fetch_sub(sequence + 1, 1);
    return pthread_mutex_lock(mutex);
}
int pthread_cond_signal(void* condition)
{
    atomic_fetch_add_explicit(
        (_Atomic unsigned*)condition, 1, memory_order_release);
    nrt_futex(condition, 129, 1);
    return 0;
}
int pthread_cond_broadcast(void* condition)
{
    atomic_fetch_add_explicit(
        (_Atomic unsigned*)condition, 1, memory_order_release);
    nrt_futex(condition, 129, INT_MAX);
    return 0;
}
#    pragma GCC visibility pop
#endif
#endif
