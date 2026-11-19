#include "../../data/nrt.c"
#define CHECK(condition)                                                       \
    do {                                                                       \
        if (!(condition))                                                      \
            nrt_abort("native startup check failed", 26);                      \
    } while (0)
_Alignas(65536) static _Thread_local volatile uint64_t aligned_tls = 9;
static int                               initialised;
static uintptr_t                         main_arena;
__attribute__((constructor)) static void initialise(void)
{
    void* p = nrt_mem_alloc(33, 32, NULL, 0);
    CHECK(p != NULL);
    nrt_mem_free(p);
    initialised = 42;
    // The runner starts with NO_COLOR=1 and TERM=dumb. Constructor mutation
    // must not invalidate the policy copied directly from the initial stack.
    for (char** e = nrt_environment; *e; ++e) {
        const char* value = nrt_environment_value(*e, "NO_COLOR");
        if (value && *value) {
            ((char*)value)[0] = 0;
        }
        value = nrt_environment_value(*e, "TERM");
        if (value && *value) {
            ((char*)value)[0] = 'x';
        }
    }
}
__attribute__((destructor)) static void finalise(void)
{
    CHECK(initialised == 42);
    nrt_prn((const u8*)"native fini passed", 18);
}
static void* worker(void* context)
{
    nrt_thread_init();
    CHECK(nrt_errno == 0);
    nrt_errno = 17;
    CHECK((uintptr_t)nrt_temp_arena() != main_arena);
    CHECK(pthread_join(pthread_self(), NULL) == 35);
    CHECK(nrt_tls_anchor == 1);
    nrt_tls_anchor = 7;
    CHECK(aligned_tls == 9 && ((uintptr_t)&aligned_tls % 65536) == 0);
    aligned_tls = 10;
    if (context == (void*)1) {
        nrt_abort("worker fatal", 12);
    }
    if (context == (void*)2) {
        nrt_signal_abort();
    }
    nrt_thread_done();
    return (void*)42;
}
__attribute__((noinline)) static int corrupt_guard(void)
{
    volatile u8 buffer[32];
    buffer[0] = 1;
#if defined(__x86_64__)
    __asm__ volatile("xorq $1,%%fs:40" ::: "memory");
#else
    __stack_chk_guard ^= 1;
#endif
    return buffer[0];
}
int main(int argc, char** argv)
{
    CHECK(initialised == 42);
    CHECK(argc >= 1 && argv[argc] == NULL);
    CHECK(g_nrt_process_metadata.no_colour &&
          g_nrt_process_metadata.dumb_terminal);
    CHECK(nrt_page_size() == g_nrt_process_metadata.page_size &&
          nrt_page_size() > 0);
    CHECK(nrt_tls_anchor == 1);
    nrt_tls_anchor = 3;
    nrt_errno      = 2;
    CHECK(aligned_tls == 9 && ((uintptr_t)&aligned_tls % 65536) == 0);
    aligned_tls = 11;
    nrt_core_init();
    main_arena = (uintptr_t)nrt_temp_arena();
    if (argc > 1 && nrt_cstr_equal(argv[1], "abort")) {
        nrt_signal_abort();
    }
    if (argc > 1 && nrt_cstr_equal(argv[1], "guard")) {
        return corrupt_guard();
    }
    void* context =
        argc > 1 && nrt_cstr_equal(argv[1], "worker-fatal")   ? (void*)1
        : argc > 1 && nrt_cstr_equal(argv[1], "worker-abort") ? (void*)2
                                                              : NULL;
    for (int i = 0; i < 32; ++i) {
        uintptr_t handle = 0;
        void*     result = NULL;
        CHECK(pthread_create(&handle, NULL, worker, context) == 0);
        CHECK(pthread_join(handle, &result) == 0 && result == (void*)42);
        CHECK(nrt_errno == 2 && nrt_tls_anchor == 3 && aligned_tls == 11);
    }
    nrt_core_done();
    nrt_prn((const u8*)"native startup passed", 21);
    return 42;
}
