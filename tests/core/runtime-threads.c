// Native runtime regression: concurrent tracking, cross-thread ownership and
// TLS cleanup.
#include "../../data/nrt.c"
#include <stdatomic.h>
#ifndef _WIN32
#    include <pthread.h>
#endif
#define CHECK(x)                                                               \
    do {                                                                       \
        if (!(x)) {                                                            \
            fprintf(stderr, "check failed at %d: %s\n", __LINE__, #x);         \
            abort();                                                           \
        }                                                                      \
    } while (0)
#define WORKERS 8
static atomic_int  ready;
static atomic_bool start;
typedef struct {
    void* incoming;
    void* outgoing;
} Work;

static void work(Work* item)
{
    nrt_thread_init();
    atomic_fetch_add(&ready, 1);
    while (!atomic_load(&start)) {
    }
    CHECK(*(unsigned char*)item->incoming == 73);
    nrt_mem_free(item->incoming);
    for (unsigned i = 0; i < 128; ++i) {
        unsigned char* heap = nrt_mem_alloc(31, 16, __FILE__, __LINE__);
        memset(heap, 42, 31);
        heap = nrt_mem_realloc(heap, 97, 16, __FILE__, __LINE__);
        CHECK(heap[30] == 42 && nrt_mem_size(heap) == 97);
        if (i % 2 == 0) {
            nrt_mem_leak(heap);
        }
        nrt_mem_free(heap);
        NrtArena arena = {0};
        CHECK(nrt_arena_alloc(&arena, 5000, 16, __FILE__, __LINE__) != NULL);
        nrt_arena_reset(&arena);
        nrt_arena_done(&arena);
        nrt_string_builder_reset();
        nrt_string_builder_append_byte('x');
        NerdString text = {0};
        nrt_string_builder_finish(&text, 0);
        CHECK(text.count == 1 && text.data[0] == 'x');
    }
    item->outgoing = nrt_mem_alloc(19, 16, __FILE__, __LINE__);
    memset(item->outgoing, 91, 19);
    CHECK(g_temp_arena.base != NULL && g_string_builder_data != NULL);
    nrt_thread_done();
    CHECK(g_temp_arena.base == NULL && g_string_builder_data == NULL);
    nrt_thread_done(); // Cleanup is idempotent; it must not report other
                       // threads' allocations.
    nrt_thread_init();
    nrt_string_builder_append_byte('y');
    NerdString again = {0};
    nrt_string_builder_finish(&again, 0);
    CHECK(again.count == 1 && again.data[0] == 'y');
    nrt_thread_done();
    CHECK(g_temp_arena.base == NULL && g_string_builder_data == NULL);
}
#ifdef _WIN32
static DWORD WINAPI entry(void* value)
{
    work(value);
    return 0;
}
#else
static void* entry(void* value)
{
    work(value);
    return NULL;
}
#endif
int main(void)
{
    Work items[WORKERS] = {0};
    nrt_core_init();
    void* sentinel = nrt_mem_alloc(7, 16, __FILE__, __LINE__);
#ifdef _WIN32
    HANDLE threads[WORKERS];
#else
    pthread_t threads[WORKERS];
#endif
    for (int i = 0; i < WORKERS; ++i) {
        items[i].incoming = nrt_mem_alloc(11, 16, __FILE__, __LINE__);
        memset(items[i].incoming, 73, 11);
#ifdef _WIN32
        threads[i] = CreateThread(NULL, 0, entry, &items[i], 0, NULL);
        CHECK(threads[i] != NULL);
#else
        CHECK(pthread_create(&threads[i], NULL, entry, &items[i]) == 0);
#endif
    }
    while (atomic_load(&ready) != WORKERS) {
    }
    atomic_store(&start, true);
    for (int i = 0; i < WORKERS; ++i) {
#ifdef _WIN32
        CHECK(WaitForSingleObject(threads[i], 30000) == WAIT_OBJECT_0);
        CHECK(CloseHandle(threads[i]));
#else
        CHECK(pthread_join(threads[i], NULL) == 0);
#endif
        CHECK(((unsigned char*)items[i].outgoing)[18] == 91);
        nrt_mem_free(items[i].outgoing);
    }
#ifndef NDEBUG
    NrtHeapDebugHeader* last = nrt_mem_live_head();
    CHECK(last != NULL && last->next == NULL && last->requested_size == 7);
    CHECK(nrt_arena_live_head() == NULL);
#endif
    nrt_mem_free(sentinel);
    nrt_core_done();
    puts("runtime threads passed");
    return 0;
}
