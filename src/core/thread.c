//------------------------------------------------------------------------------
// Joinable threads and condition variables
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

#if OS_LINUX
#    include <errno.h>
#    include <sched.h>
#elif OS_POSIX
#    include <unistd.h>
#endif

#if OS_WINDOWS
#    include <process.h>

internal unsigned __stdcall thread_entry(void* argument)
{
    Thread* thread = argument;
    thread->function(thread->argument);
    return 0;
}
#else
internal void* thread_entry(void* argument)
{
    Thread* thread = argument;
    thread->function(thread->argument);
    return NULL;
}
#endif

// Count processors available to this process, not physical cores. Failure
// falls back to one rather than assuming unrestricted use of the host.
u32 thread_available_cpu_count(void)
{
#if OS_WINDOWS
    DWORD_PTR process_mask = 0, system_mask = 0;
    if (GetProcessAffinityMask(
            GetCurrentProcess(), &process_mask, &system_mask)) {
        u32 count = 0;
        while (process_mask != 0) {
            count += (u32)(process_mask & 1);
            process_mask >>= 1;
        }
        return count == 0 ? 1 : count;
    }
#elif OS_LINUX
    // Grow for sparse/high-numbered CPU IDs as well as machines above 1024
    // CPUs.
    for (usize capacity = 128; capacity <= 1048576; capacity *= 2) {
        cpu_set_t* mask = CPU_ALLOC(capacity);
        if (mask == NULL) {
            break;
        }
        usize bytes = CPU_ALLOC_SIZE(capacity);
        CPU_ZERO_S(bytes, mask);
        int result = sched_getaffinity(0, bytes, mask);
        int code   = errno;
        int count  = result == 0 ? CPU_COUNT_S(bytes, mask) : 0;
        CPU_FREE(mask);
        if (result == 0) {
            return count > 0 ? (u32)count : 1;
        }
        if (code != EINVAL) {
            break;
        }
    }
#elif defined(_SC_NPROCESSORS_ONLN)
    long count = sysconf(_SC_NPROCESSORS_ONLN);
    if (count > 0) {
        return (u64)count > U32_MAX ? U32_MAX : (u32)count;
    }
#endif
    return 1;
}

bool thread_start(Thread* thread, ThreadFunction function, void* argument)
{
    if (thread->started || function == NULL) {
        return false;
    }
    thread->function = function;
    thread->argument = argument;
#if OS_WINDOWS
    thread->handle =
        (HANDLE)_beginthreadex(NULL, 0, thread_entry, thread, 0, NULL);
    bool started = thread->handle != NULL;
#else
    bool started =
        pthread_create(&thread->handle, NULL, thread_entry, thread) == 0;
#endif
    if (!started) {
        *thread = (Thread){0};
        return false;
    }
    thread->started = true;
    return true;
}

bool thread_join(Thread* thread)
{
    if (!thread->started) {
        return true;
    }
#if OS_WINDOWS
    if (GetThreadId(thread->handle) == GetCurrentThreadId()) {
        return false;
    }
    if (WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) {
        return false;
    }
    if (!CloseHandle(thread->handle)) {
        return false;
    }
#else
    if (pthread_equal(thread->handle, pthread_self())) {
        return false;
    }
    if (pthread_join(thread->handle, NULL) != 0) {
        return false;
    }
#endif
    *thread = (Thread){0};
    return true;
}

bool condition_init(Condition* condition)
{
#if OS_WINDOWS
    InitializeConditionVariable(condition);
    return true;
#else
    return pthread_cond_init(condition, NULL) == 0;
#endif
}

void condition_done(Condition* condition)
{
#if OS_WINDOWS
    UNUSED(condition); // No native destruction is required.
#else
    pthread_cond_destroy(condition);
#endif
}

bool condition_wait(Condition* condition, Mutex* mutex)
{
#if OS_WINDOWS
    return SleepConditionVariableCS(condition, mutex, INFINITE) != 0;
#else
    return pthread_cond_wait(condition, mutex) == 0;
#endif
}

void condition_signal(Condition* condition)
{
#if OS_WINDOWS
    WakeConditionVariable(condition);
#else
    pthread_cond_signal(condition);
#endif
}

void condition_broadcast(Condition* condition)
{
#if OS_WINDOWS
    WakeAllConditionVariable(condition);
#else
    pthread_cond_broadcast(condition);
#endif
}
