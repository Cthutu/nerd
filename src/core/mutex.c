//------------------------------------------------------------------------------
// Mutex implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

#if OS_WINDOWS

void mutex_init(Mutex* mutex) { InitializeCriticalSection(mutex); }
bool mutex_init_checked(Mutex* mutex)
{
    return InitializeCriticalSectionAndSpinCount(mutex, 0) != 0;
}
void mutex_done(Mutex* mutex) { DeleteCriticalSection(mutex); }
void mutex_lock(Mutex* mutex) { EnterCriticalSection(mutex); }
void mutex_unlock(Mutex* mutex) { LeaveCriticalSection(mutex); }

#else // OS_POSIX

void mutex_init(Mutex* mutex) { pthread_mutex_init(mutex, NULL); }
bool mutex_init_checked(Mutex* mutex)
{
    return pthread_mutex_init(mutex, NULL) == 0;
}
void mutex_done(Mutex* mutex) { pthread_mutex_destroy(mutex); }
void mutex_lock(Mutex* mutex) { pthread_mutex_lock(mutex); }
void mutex_unlock(Mutex* mutex) { pthread_mutex_unlock(mutex); }

#endif // OS_WINDOWS
