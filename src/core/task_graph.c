// Persistent workers and dependency-ready work-stealing queues.
// Copyright (C)2026 Matt Davies, all rights reserved
#include <core/core.h>

#define TASK_NONE ((usize) - 1)

typedef struct {
    usize pending, first_edge, previous, next;
} TaskNodeState;
typedef struct {
    usize target, next;
} TaskGraphEdge;
typedef struct {
    usize first, last;
} TaskDeque;
typedef struct {
    const TaskNode* nodes;
    TaskNodeState*  states;
    TaskGraphEdge*  edges;
    usize           remaining;
    bool            failed;
} TaskGraph;
typedef struct {
    TaskPool* pool;
    u32       index;
} TaskWorker;

struct TaskPool {
    Mutex      mutex;
    Condition  changed;
    Thread     threads[TASK_MAX_JOBS - 1];
    TaskWorker workers[TASK_MAX_JOBS - 1];
    TaskDeque  queues[TASK_MAX_JOBS];
    TaskGraph* graph;
    u64        generation;
    u32        jobs, started, finished;
    bool       closing;
};

// All queue operations and completion publication hold the pool mutex.
internal void task_deque_push(TaskPool* pool, u32 worker, usize node)
{
    TaskDeque*     queue = &pool->queues[worker];
    TaskNodeState* state = &pool->graph->states[node];
    state->previous      = queue->last;
    state->next          = TASK_NONE;
    if (queue->last != TASK_NONE) {
        pool->graph->states[queue->last].next = node;
    } else {
        queue->first = node;
    }
    queue->last = node;
}

internal usize task_deque_take(TaskPool* pool, u32 worker, bool steal)
{
    TaskDeque* queue = &pool->queues[worker];
    usize      node  = steal ? queue->first : queue->last;
    if (node == TASK_NONE) {
        return node;
    }
    TaskNodeState* state = &pool->graph->states[node];
    if (state->previous != TASK_NONE) {
        pool->graph->states[state->previous].next = state->next;
    } else {
        queue->first = state->next;
    }
    if (state->next != TASK_NONE) {
        pool->graph->states[state->next].previous = state->previous;
    } else {
        queue->last = state->previous;
    }
    return node;
}

// Enter and return with the mutex locked. Idle workers sleep; only ready nodes
// execute. A callback's writes happen-before publication of its dependents.
internal void task_graph_work(TaskPool* pool, u32 worker)
{
    TaskGraph* graph = pool->graph;
    while (!graph->failed && graph->remaining != 0) {
        usize node = task_deque_take(pool, worker, false);
        for (u32 offset = 1; node == TASK_NONE && offset < pool->jobs;
             ++offset) {
            node = task_deque_take(pool, (worker + offset) % pool->jobs, true);
        }
        if (node == TASK_NONE) {
            if (!condition_wait(&pool->changed, &pool->mutex)) {
                abort();
            }
            continue;
        }
        TaskNode task = graph->nodes[node];
        mutex_unlock(&pool->mutex);
        bool ok = task.function(task.context, task.index);
        mutex_lock(&pool->mutex);
        graph->remaining--;
        if (!ok) {
            graph->failed = true;
        }
        if (!graph->failed) {
            for (usize edge = graph->states[node].first_edge; edge != TASK_NONE;
                 edge       = graph->edges[edge].next) {
                usize target = graph->edges[edge].target;
                if (--graph->states[target].pending == 0) {
                    task_deque_push(pool, worker, target);
                }
            }
        }
        condition_broadcast(&pool->changed);
    }
}

internal void task_pool_worker(void* argument)
{
    TaskWorker* worker = argument;
    TaskPool*   pool   = worker->pool;
    u64         seen   = 0;
    mutex_lock(&pool->mutex);
    for (;;) {
        while (!pool->closing && seen == pool->generation) {
            if (!condition_wait(&pool->changed, &pool->mutex)) {
                abort();
            }
        }
        if (pool->closing) {
            break;
        }
        seen = pool->generation;
        task_graph_work(pool, worker->index);
        pool->finished++;
        condition_broadcast(&pool->changed);
    }
    mutex_unlock(&pool->mutex);
}

TaskPool* task_pool_create(u32 jobs)
{
    if (jobs == 0 || jobs > TASK_MAX_JOBS) {
        return NULL;
    }
    TaskPool* pool = calloc(1, sizeof(*pool));
    if (pool == NULL) {
        return NULL;
    }
    if (!mutex_init_checked(&pool->mutex)) {
        free(pool);
        return NULL;
    }
    if (!condition_init(&pool->changed)) {
        mutex_done(&pool->mutex);
        free(pool);
        return NULL;
    }
    pool->jobs = jobs;
    for (; pool->started < jobs - 1; ++pool->started) {
        u32 slot            = pool->started;
        pool->workers[slot] = (TaskWorker){pool, slot + 1};
        if (!thread_start(
                &pool->threads[slot], task_pool_worker, &pool->workers[slot])) {
            task_pool_destroy(pool);
            return NULL;
        }
    }
    return pool;
}

void task_pool_destroy(TaskPool* pool)
{
    if (pool == NULL) {
        return;
    }
    mutex_lock(&pool->mutex);
    // Destruction is a coordinator operation, never concurrent with run.
    if (pool->graph != NULL) {
        abort();
    }
    pool->closing = true;
    condition_broadcast(&pool->changed);
    mutex_unlock(&pool->mutex);
    for (u32 i = 0; i < pool->started; ++i) {
        if (!thread_join(&pool->threads[i])) {
            abort();
        }
    }
    condition_done(&pool->changed);
    mutex_done(&pool->mutex);
    free(pool);
}

internal TaskRunStatus task_graph_prepare(TaskGraph*      graph,
                                          const TaskNode* nodes,
                                          usize           count,
                                          const TaskEdge* edges,
                                          usize           edge_count)
{
    if ((count != 0 && nodes == NULL) || (edge_count != 0 && edges == NULL)) {
        return TASK_RUN_INVALID_GRAPH;
    }
    graph->nodes     = nodes;
    graph->remaining = count;
    graph->states    = count ? calloc(count, sizeof(*graph->states)) : NULL;
    graph->edges =
        edge_count ? calloc(edge_count, sizeof(*graph->edges)) : NULL;
    usize*        ready  = count ? calloc(count, sizeof(*ready)) : NULL;
    TaskRunStatus status = TASK_RUN_INVALID_GRAPH;
    if ((count && (!graph->states || !ready)) ||
        (edge_count && !graph->edges)) {
        status = TASK_RUN_START_FAILED;
        goto done;
    }
    for (usize i = 0; i < count; ++i) {
        if (nodes[i].function == NULL) {
            goto done;
        }
        graph->states[i].first_edge = TASK_NONE;
    }
    for (usize i = 0; i < edge_count; ++i) {
        if (edges[i].before >= count || edges[i].after >= count) {
            goto done;
        }
        TaskNodeState* before = &graph->states[edges[i].before];
        graph->edges[i] = (TaskGraphEdge){edges[i].after, before->first_edge};
        before->first_edge = i;
        graph->states[edges[i].after].pending++;
    }
    // Validate acyclicity before allowing any side effects. Duplicate edges are
    // permitted; their dependency counts are also decremented independently.
    usize head = 0, tail = 0;
    for (usize i = 0; i < count; ++i) {
        if (graph->states[i].pending == 0) {
            ready[tail++] = i;
        }
    }
    while (head < tail) {
        usize node = ready[head++];
        for (usize edge = graph->states[node].first_edge; edge != TASK_NONE;
             edge       = graph->edges[edge].next) {
            usize target = graph->edges[edge].target;
            if (--graph->states[target].pending == 0) {
                ready[tail++] = target;
            }
        }
    }
    if (tail != count) {
        goto done;
    }
    for (usize i = 0; i < edge_count; ++i) {
        graph->states[edges[i].after].pending++;
    }
    status = TASK_RUN_OK;
done:
    free(ready);
    return status;
}

TaskRunStatus task_pool_run(TaskPool*       pool,
                            const TaskNode* nodes,
                            usize           count,
                            const TaskEdge* edges,
                            usize           edge_count)
{
    if (pool == NULL) {
        return TASK_RUN_START_FAILED;
    }
    TaskGraph     graph = {0};
    TaskRunStatus status =
        task_graph_prepare(&graph, nodes, count, edges, edge_count);
    if (status != TASK_RUN_OK) {
        goto done;
    }
    mutex_lock(&pool->mutex);
    if (pool->graph != NULL || pool->closing) {
        mutex_unlock(&pool->mutex);
        status = TASK_RUN_START_FAILED;
        goto done;
    }
    pool->graph    = &graph;
    pool->finished = 0;
    for (u32 i = 0; i < pool->jobs; ++i) {
        pool->queues[i] = (TaskDeque){TASK_NONE, TASK_NONE};
    }
    u32 destination = 0;
    for (usize i = 0; i < count; ++i) {
        if (graph.states[i].pending == 0) {
            task_deque_push(pool, destination++ % pool->jobs, i);
        }
    }
    pool->generation++;
    condition_broadcast(&pool->changed);
    task_graph_work(pool, 0);
    while (pool->finished != pool->started) {
        if (!condition_wait(&pool->changed, &pool->mutex)) {
            abort();
        }
    }
    status      = graph.failed ? TASK_RUN_FAILED : TASK_RUN_OK;
    pool->graph = NULL;
    mutex_unlock(&pool->mutex);
done:
    free(graph.states);
    free(graph.edges);
    return status;
}
