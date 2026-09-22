//------------------------------------------------------------------------------
// Bounded task batch: a finite index queue and joinable workers
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
#include <core/core.h>

u32 task_auto_jobs(u32 available_cpus)
{
    u32 jobs = available_cpus / 2;
    return jobs == 0 ? 1 : jobs > TASK_MAX_JOBS ? TASK_MAX_JOBS : jobs;
}

typedef struct {
    Mutex        mutex;
    Condition    changed;
    usize        next;
    usize        count;
    bool         ready;
    bool         stop;
    TaskFunction function;
    void*        context;
} TaskBatch;

internal void task_worker(void* argument)
{
    TaskBatch* batch = argument;
    mutex_lock(&batch->mutex);
    while (!batch->ready && !batch->stop) {
        if (!condition_wait(&batch->changed, &batch->mutex)) {
            abort();
        }
    }
    while (!batch->stop && batch->next < batch->count) {
        usize index = batch->next++;
        mutex_unlock(&batch->mutex);
        bool ok = batch->function(batch->context, index);
        mutex_lock(&batch->mutex);
        if (!ok) {
            batch->stop = true;
        }
    }
    mutex_unlock(&batch->mutex);
}

TaskRunStatus
task_run(usize count, u32 jobs, TaskFunction function, void* context)
{
    if (jobs == 0 || jobs > TASK_MAX_JOBS || function == NULL) {
        return TASK_RUN_START_FAILED;
    }
    if (count == 0) {
        return TASK_RUN_OK;
    }
    if (jobs > count) {
        jobs = (u32)count;
    }
    if (jobs == 1) {
        for (usize i = 0; i < count; ++i) {
            if (!function(context, i)) {
                return TASK_RUN_FAILED;
            }
        }
        return TASK_RUN_OK;
    }
    TaskBatch batch = {
        .count = count, .function = function, .context = context};
    if (!mutex_init_checked(&batch.mutex)) {
        return TASK_RUN_START_FAILED;
    }
    if (!condition_init(&batch.changed)) {
        mutex_done(&batch.mutex);
        return TASK_RUN_START_FAILED;
    }
    // Slots never move while workers hold pointers into them. No per-task
    // allocation or unbounded producer queue is needed for a frozen batch.
    Thread workers[TASK_MAX_JOBS - 1] = {0};
    u32    started                    = 0;
    for (; started < jobs - 1; ++started) {
        if (!thread_start(&workers[started], task_worker, &batch)) {
            break;
        }
    }
    bool start_failed = started != jobs - 1;
    mutex_lock(&batch.mutex);
    batch.stop  = start_failed;
    batch.ready = true;
    condition_broadcast(&batch.changed);
    mutex_unlock(&batch.mutex);
    if (!start_failed) {
        task_worker(&batch);
    }
    for (u32 i = 0; i < started; ++i) {
        // Returning with a live worker would invalidate stack/input storage.
        if (!thread_join(&workers[i])) {
            abort();
        }
    }
    TaskRunStatus status = start_failed ? TASK_RUN_START_FAILED
                           : batch.stop ? TASK_RUN_FAILED
                                        : TASK_RUN_OK;
    condition_done(&batch.changed);
    mutex_done(&batch.mutex);
    return status;
}
