// Standalone allocator stress test: no compiler globals or scheduler required.
#include <core/core.h>
#undef NDEBUG
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <time.h>

enum { WORKERS = 4, SLOTS = 2000 };
static void*       blocks[WORKERS][SLOTS];
static atomic_bool finished;
static int         phase;

// memory.c's failure/reporting paths only; avoid the compiler output subsystem.
void eprn(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
}

static void work(usize id)
{
    if (id == WORKERS) {
        MemoryStats previous = mem_stats_snapshot();
        while (!atomic_load(&finished)) {
            MemoryStats now = mem_stats_snapshot();
            assert(now.heap_peak_bytes >= now.heap_current_bytes);
            assert(now.heap_peak_bytes >= previous.heap_peak_bytes);
            assert(now.heap_alloc_count >= previous.heap_alloc_count);
#if CONFIG_DEBUG
            (void)mem_get_allocation_count();
            (void)mem_get_total_allocated();
#endif
            previous = now;
#if OS_WINDOWS
            Sleep(1);
#else
            struct timespec delay = {.tv_nsec = 100000};
            nanosleep(&delay, NULL);
#endif
        }
        return;
    }
    MemoryStats activity_before = mem_stats_thread_snapshot();
    // Each phase hands allocations to a different thread after join.
    usize       owner           = (id + phase) % WORKERS;
    for (usize i = 0; i < SLOTS; ++i) {
        void** block = &blocks[owner][i];
        if (phase == 0) {
            *block = mem_alloc(32, __FILE__, __LINE__);
            assert((uintptr_t)*block % _Alignof(max_align_t) == 0);
            memset(*block, 0x5a, 32);
        } else if (phase == 1) {
            if (i % 2 == 0) {
                mem_leak(*block);
                mem_leak(*block);
            }
            *block = mem_realloc(*block, 64, __FILE__, __LINE__);
            assert((uintptr_t)*block % _Alignof(max_align_t) == 0);
            assert(mem_size(*block) == 64);
            for (usize j = 0; j < 32; ++j) {
                assert(((u8*)*block)[j] == 0x5a);
            }
            // Exercise shrink accounting as well as growth.
            *block = mem_realloc(*block, 48, __FILE__, __LINE__);
        } else {
            *block = mem_free(*block, __FILE__, __LINE__);
            assert(*block == NULL);
            void* scratch = mem_realloc(NULL, 16, __FILE__, __LINE__);
            scratch       = mem_realloc(scratch, 80, __FILE__, __LINE__);
            mem_free(scratch, __FILE__, __LINE__);
            mem_stats_record_arena_init(4096);
            mem_stats_record_arena_alloc(17);
            mem_stats_record_arena_commit(4096);
            mem_stats_record_arena_done();
            mem_stats_record_array_growth(64);
        }
    }
    MemoryStats activity =
        mem_stats_delta(activity_before, mem_stats_thread_snapshot());
    assert(activity.heap_current_bytes == 0 && activity.heap_peak_bytes == 0);
    if (phase == 0) {
        assert(activity.heap_alloc_count == SLOTS);
        assert(activity.heap_bytes_allocated == 32 * SLOTS);
        assert(activity.heap_realloc_count == 0 &&
               activity.heap_free_count == 0);
    } else if (phase == 1) {
        assert(activity.heap_alloc_count == 0 && activity.heap_free_count == 0);
        assert(activity.heap_realloc_count == 2 * SLOTS);
        assert(activity.heap_bytes_reallocated == (64 + 48) * SLOTS);
    } else {
        assert(activity.heap_alloc_count == SLOTS);
        assert(activity.heap_realloc_count == SLOTS);
        assert(activity.heap_free_count == 2 * SLOTS);
        assert(activity.heap_bytes_freed == (48 + 80) * SLOTS);
        assert(activity.arena_init_count == SLOTS &&
               activity.arena_done_count == SLOTS);
        assert(activity.arena_alloc_count == SLOTS &&
               activity.arena_bytes_allocated == 17 * SLOTS);
        assert(activity.arena_commit_count == 2 * SLOTS &&
               activity.arena_bytes_committed == 8192 * SLOTS);
        assert(activity.array_growth_count == SLOTS &&
               activity.array_bytes_allocated == 64 * SLOTS);
    }
}

#if OS_WINDOWS
static DWORD WINAPI entry(void* arg)
{
    work((usize)arg);
    return 0;
}
typedef HANDLE    TestThread;
static TestThread start(usize id)
{
    HANDLE thread = CreateThread(NULL, 0, entry, (void*)id, 0, NULL);
    assert(thread != NULL);
    return thread;
}
static void join(TestThread thread)
{
    assert(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    assert(CloseHandle(thread));
}
#else
static void* entry(void* arg)
{
    work((usize)arg);
    return NULL;
}
typedef pthread_t TestThread;
static TestThread start(usize id)
{
    pthread_t thread;
    assert(pthread_create(&thread, NULL, entry, (void*)id) == 0);
    return thread;
}
static void join(TestThread thread) { assert(pthread_join(thread, NULL) == 0); }
#endif

int main(void)
{
    // Keep assertions active even when memory.c is compiled with NDEBUG.
#if CONFIG_DEBUG
    void* reported = mem_alloc(7, __FILE__, __LINE__);
    mem_print_leaks();
    mem_free(reported, __FILE__, __LINE__);
    mem_print_leaks();
#endif
    MemoryStats before       = mem_stats_snapshot();
    MemoryStats local_before = mem_stats_thread_snapshot();
    for (phase = 0; phase < 3; ++phase) {
        atomic_store(&finished, false);
        TestThread observer = start(WORKERS);
        TestThread threads[WORKERS];
        for (usize i = 0; i < WORKERS; ++i) {
            threads[i] = start(i);
        }
        for (usize i = 0; i < WORKERS; ++i) {
            join(threads[i]);
        }
        atomic_store(&finished, true);
        join(observer);
#if CONFIG_DEBUG
        usize expected = phase == 0   ? WORKERS * SLOTS
                         : phase == 1 ? WORKERS * SLOTS / 2
                                      : 0;
        assert(mem_get_allocation_count() == expected);
        assert(mem_get_total_allocated() == expected * (phase == 0 ? 32 : 48));
#endif
    }
    MemoryStats local_delta =
        mem_stats_delta(local_before, mem_stats_thread_snapshot());
    assert(local_delta.heap_alloc_count == 0 &&
           local_delta.heap_free_count == 0);
    assert(local_delta.heap_realloc_count == 0 &&
           local_delta.arena_alloc_count == 0);
    MemoryStats after = mem_stats_snapshot();
    MemoryStats delta = mem_stats_delta(before, after);
    usize       n     = WORKERS * SLOTS;
    assert(delta.heap_alloc_count == 2 * n);
    assert(delta.heap_realloc_count == 3 * n);
    assert(delta.heap_free_count == 2 * n);
    assert(delta.heap_bytes_allocated == 48 * n);
    assert(delta.heap_bytes_reallocated == (64 + 48 + 80) * n);
    assert(delta.heap_bytes_freed == (48 + 80) * n);
    assert(after.heap_current_bytes == before.heap_current_bytes);
    assert(after.heap_peak_bytes >= 48 * n);
    assert(delta.arena_init_count == n && delta.arena_done_count == n);
    assert(delta.arena_alloc_count == n &&
           delta.arena_bytes_allocated == 17 * n);
    assert(delta.arena_commit_count == 2 * n &&
           delta.arena_bytes_committed == 8192 * n);
    assert(delta.array_growth_count == n &&
           delta.array_bytes_allocated == 64 * n);
    assert(mem_free(NULL, __FILE__, __LINE__) == NULL && mem_size(NULL) == 0);
    puts("memory-concurrency ok");
}
