// Lifecycle/predicate tests for the production primitives, not a compiler pool.
#include <core/core.h>
#undef NDEBUG
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>

#ifdef NERD_TEST_START_FAILURE
void thread_test_fail_after(int count);
#endif

enum { WORKERS = 4, ROUNDS = 100 };
typedef struct {
    Mutex     mutex;
    Condition work;
    Condition changed;
    bool      stop;
    usize     generation;
    usize     ready;
    usize     completed;
    usize     totals[WORKERS];
} Shared;
typedef struct {
    Shared* shared;
    usize   id;
} Argument;

static void worker(void* opaque)
{
    Argument* argument = opaque;
    Shared*   shared   = argument->shared;
    usize     seen     = 0;
    mutex_lock(&shared->mutex);
    shared->ready++;
    condition_signal(&shared->changed);
    for (;;) {
        while (!shared->stop && shared->generation == seen) {
            assert(condition_wait(&shared->work, &shared->mutex));
        }
        if (shared->stop) {
            break;
        }
        seen = shared->generation;
        mutex_unlock(&shared->mutex);
        // Each worker owns a result slot. Join or the completion predicate
        // publishes results before the coordinator reads or releases them.
        shared->totals[argument->id] += seen;
        mutex_lock(&shared->mutex);
        shared->completed++;
        condition_signal(&shared->changed);
    }
    mutex_unlock(&shared->mutex);
}

static void run_batch(usize count, bool reject_start, bool stop_with_work)
{
    Shared shared = {0};
    assert(mutex_init_checked(&shared.mutex));
    assert(condition_init(&shared.work));
    assert(condition_init(&shared.changed));
    Thread   threads[WORKERS] = {0};
    Argument arguments[WORKERS];
    usize    started = 0;
#ifdef NERD_TEST_START_FAILURE
    if (reject_start) {
        thread_test_fail_after(2);
    }
#endif
    for (; started < count; ++started) {
        arguments[started] = (Argument){&shared, started};
        if (reject_start && started == 2) {
#ifdef NERD_TEST_START_FAILURE
            assert(
                !thread_start(&threads[started], worker, &arguments[started]));
#else
            assert(!thread_start(&threads[started], NULL, &arguments[started]));
#endif
            assert(!threads[started].started &&
                   threads[started].function == NULL);
            break;
        }
        assert(thread_start(&threads[started], worker, &arguments[started]));
        assert(!thread_start(&threads[started], worker, &arguments[started]));
    }
    mutex_lock(&shared.mutex);
    if (!reject_start) {
        while (shared.ready != started) {
            assert(condition_wait(&shared.changed, &shared.mutex));
        }
        for (usize round = 1; round <= ROUNDS; ++round) {
            // Extra notifications must never be treated as additional jobs.
            condition_broadcast(&shared.work);
            shared.completed  = 0;
            shared.generation = round;
            condition_broadcast(&shared.work);
            while (shared.completed != started) {
                assert(condition_wait(&shared.changed, &shared.mutex));
            }
            for (usize i = 0; i < started; ++i) {
                assert(shared.totals[i] == round * (round + 1) / 2);
            }
        }
    }
    if (stop_with_work) {
        shared.generation++;
        condition_broadcast(&shared.work);
        mutex_unlock(&shared.mutex);
        mutex_lock(&shared.mutex);
    }
    shared.stop = true;
    condition_broadcast(&shared.work);
    mutex_unlock(&shared.mutex);
    for (usize i = 0; i < WORKERS; ++i) {
        assert(thread_join(&threads[i]));
        assert(thread_join(&threads[i]));
        assert(!threads[i].started && threads[i].function == NULL);
    }
    condition_done(&shared.changed);
    condition_done(&shared.work);
    mutex_done(&shared.mutex);
}

static void set_value(void* argument) { *(usize*)argument = 42; }

typedef struct {
    usize       visits[128];
    atomic_uint active;
    atomic_uint peak;
    bool        fail;
} BatchTest;

static bool task_test(void* argument, usize index)
{
    BatchTest* test   = argument;
    unsigned   active = atomic_fetch_add(&test->active, 1) + 1;
    unsigned   peak   = atomic_load(&test->peak);
    while (peak < active &&
           !atomic_compare_exchange_weak(&test->peak, &peak, active)) {
    }
    test->visits[index]++;
    atomic_fetch_sub(&test->active, 1);
    return !test->fail;
}

static void batch_test(void)
{
    for (u32 jobs = 1; jobs <= 8; jobs *= 2) {
        for (usize repeat = 0; repeat < 20; ++repeat) {
            BatchTest test = {0};
            assert(task_run(128, jobs, task_test, &test) == TASK_RUN_OK);
            for (usize i = 0; i < 128; ++i) {
                assert(test.visits[i] == 1);
            }
            assert(atomic_load(&test.active) == 0);
            assert(atomic_load(&test.peak) <= jobs);
            test = (BatchTest){.fail = true};
            assert(task_run(128, jobs, task_test, &test) == TASK_RUN_FAILED);
            usize calls = 0;
            for (usize i = 0; i < 128; ++i) {
                calls += test.visits[i];
            }
            assert(calls > 0 && calls <= jobs &&
                   atomic_load(&test.active) == 0);
        }
    }
    BatchTest test = {0};
    assert(task_run(0, 4, task_test, &test) == TASK_RUN_OK);
    assert(task_run(1, TASK_MAX_JOBS, task_test, &test) == TASK_RUN_OK);
    assert(task_run(1, 0, task_test, &test) == TASK_RUN_START_FAILED);
    assert(task_run(1, TASK_MAX_JOBS + 1, task_test, &test) ==
           TASK_RUN_START_FAILED);
#ifdef NERD_TEST_START_FAILURE
    test = (BatchTest){0};
    thread_test_fail_after(2);
    assert(task_run(128, 4, task_test, &test) == TASK_RUN_START_FAILED);
    // Startup is transactional: no work begins until every worker starts.
    for (usize i = 0; i < 128; ++i) {
        assert(test.visits[i] == 0);
    }
    assert(task_run(128, 4, task_test, &test) == TASK_RUN_OK);
#endif
}

int main(void)
{
    Thread reused = {0};
    assert(thread_join(&reused));
    assert(!thread_start(&reused, NULL, NULL));
    for (usize i = 0; i < 50; ++i) {
        usize value = 0;
        assert(thread_start(&reused, set_value, &value));
        assert(thread_join(&reused));
        assert(value == 42);
    }
    for (usize i = 0; i < 10; ++i) {
        run_batch(1, false, false);
        run_batch(WORKERS, false, i % 2 == 0);
        run_batch(WORKERS, true, false);
    }
    batch_test();
    puts("thread-lifecycle ok");
}
