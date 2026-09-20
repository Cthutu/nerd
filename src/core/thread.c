//------------------------------------------------------------------------------
// Joinable threads and condition variables
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

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
