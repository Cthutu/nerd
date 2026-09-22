#include <core/core.h>
#undef NDEBUG
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>

enum { WIDTH = 24, NODES = WIDTH * 3, EDGES = WIDTH * 4 };
typedef struct {
    TaskEdge    edges[EDGES];
    usize       values[NODES];
    atomic_uint visits[NODES];
} Context;

static bool evaluate(void* opaque, usize node)
{
    Context* context = opaque;
    assert(atomic_fetch_add(&context->visits[node], 1) == 0);
    usize value = node + 1;
    for (usize i = 0; i < EDGES; ++i) {
        if (context->edges[i].after == node) {
            value += context->values[context->edges[i].before];
        }
    }
    context->values[node] = value;
    return true;
}

static void exercise(TaskPool* pool)
{
    Context  context = {0};
    TaskNode nodes[NODES];
    usize    expected[NODES];
    for (usize i = 0; i < NODES; ++i) {
        nodes[i] = (TaskNode){evaluate, &context, i};
    }
    usize edge = 0;
    for (usize i = WIDTH; i < NODES; ++i) {
        context.edges[edge++] = (TaskEdge){i - WIDTH, i};
        context.edges[edge++] =
            (TaskEdge){(i % 3 == 0 ? i - WIDTH
                                   : (i / WIDTH - 1) * WIDTH + (i + 1) % WIDTH),
                       i};
    }
    assert(edge == EDGES);
    for (usize i = 0; i < NODES; ++i) {
        expected[i] = i + 1;
        for (usize j = 0; j < EDGES; ++j) {
            if (context.edges[j].after == i) {
                expected[i] += expected[context.edges[j].before];
            }
        }
    }
    assert(task_pool_run(pool, nodes, NODES, context.edges, EDGES) ==
           TASK_RUN_OK);
    for (usize i = 0; i < NODES; ++i) {
        assert(atomic_load(&context.visits[i]) == 1);
        assert(context.values[i] == expected[i]);
    }
}

static bool fail(void* opaque, usize node)
{
    atomic_uint* calls = opaque;
    atomic_fetch_add(calls, 1);
    assert(node == 0);
    return false;
}

// Prove two ready tasks can overlap, and failure waits for the other callback.
typedef struct {
    Mutex     mutex;
    Condition changed;
    u32       entered, exited;
} Drain;
static bool drain(void* opaque, usize index)
{
    Drain* state = opaque;
    if (index == 3) {
        return true; // Release both ready callbacks onto this worker's deque.
    }
    assert(index < 2);
    mutex_lock(&state->mutex);
    state->entered++;
    condition_broadcast(&state->changed);
    while (state->entered != 2) {
        assert(condition_wait(&state->changed, &state->mutex));
    }
    state->exited++;
    mutex_unlock(&state->mutex);
    return index != 0;
}

int main(void)
{
    assert(task_pool_create(0) == NULL);
    assert(task_pool_create(TASK_MAX_JOBS + 1) == NULL);
    task_pool_destroy(NULL);
    for (u32 jobs = 1; jobs <= 8; jobs *= 2) {
        TaskPool* pool = task_pool_create(jobs);
        assert(pool != NULL);
        for (usize iteration = 0; iteration < 30; ++iteration) {
            exercise(pool);
        }
        assert(task_pool_run(pool, NULL, 0, NULL, 0) == TASK_RUN_OK);
        atomic_uint calls   = 0;
        TaskNode    nodes[] = {{fail, &calls, 0}, {fail, &calls, 1}};
        TaskEdge    chain   = {0, 1};
        assert(task_pool_run(pool, nodes, 2, &chain, 1) == TASK_RUN_FAILED);
        assert(atomic_load(&calls) == 1);
        atomic_store(&calls, 0);
        TaskEdge cycle[] = {{0, 1}, {1, 0}};
        assert(task_pool_run(pool, nodes, 2, cycle, 2) ==
               TASK_RUN_INVALID_GRAPH);
        TaskNode disconnected[] = {
            {fail, &calls, 0}, {fail, &calls, 1}, {fail, &calls, 2}};
        TaskEdge partial_cycle[] = {{1, 2}, {2, 1}};
        assert(task_pool_run(pool, disconnected, 3, partial_cycle, 2) ==
               TASK_RUN_INVALID_GRAPH);
        TaskNode missing_callback = {0};
        assert(task_pool_run(pool, &missing_callback, 1, NULL, 0) ==
               TASK_RUN_INVALID_GRAPH);
        TaskEdge invalid = {0, 2};
        assert(task_pool_run(pool, nodes, 2, &invalid, 1) ==
               TASK_RUN_INVALID_GRAPH);
        assert(task_pool_run(pool, NULL, 2, NULL, 0) == TASK_RUN_INVALID_GRAPH);
        assert(task_pool_run(pool, nodes, 2, NULL, 1) ==
               TASK_RUN_INVALID_GRAPH);
        assert(atomic_load(&calls) == 0);
        exercise(pool);
        if (jobs > 1) {
            Drain state = {0};
            assert(mutex_init_checked(&state.mutex));
            assert(condition_init(&state.changed));
            TaskNode pair[]         = {{drain, &state, 0},
                                       {drain, &state, 1},
                                       {drain, &state, 2},
                                       {drain, &state, 3}};
            TaskEdge dependencies[] = {{3, 0}, {3, 1}, {0, 2}, {1, 2}};
            assert(task_pool_run(pool, pair, 4, dependencies, 4) ==
                   TASK_RUN_FAILED);
            assert(state.entered == 2 && state.exited == 2);
            condition_done(&state.changed);
            mutex_done(&state.mutex);
            exercise(pool);
        }
#ifdef NERD_TEST_START_FAILURE
        extern void thread_test_fail_after(int count);
        thread_test_fail_after(0);
        exercise(pool); // Reusing a pool must not create another native thread.
        assert(task_pool_create(2) == NULL);
        thread_test_fail_after(-1);
#endif
        task_pool_destroy(pool);
    }
#ifdef NERD_TEST_START_FAILURE
    for (int count = 0; count < 3; ++count) {
        extern void thread_test_fail_after(int count);
        thread_test_fail_after(count);
        assert(task_pool_create(4) == NULL);
    }
#endif
    puts("task-graph ok");
}
