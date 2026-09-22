// Compile the production implementation with a native-create wrapper so the
// failure path is deterministic without exhausting host thread resources.
#include <core/core.h>
#if OS_WINDOWS
#    include <process.h>
#else
#    include <errno.h>
#endif

static int  starts_until_failure = -1;
void        thread_test_fail_after(int count) { starts_until_failure = count; }
static bool reject_create(void)
{
    if (starts_until_failure < 0) {
        return false;
    }
    if (starts_until_failure-- != 0) {
        return false;
    }
    starts_until_failure = -1;
    return true;
}

#if OS_WINDOWS
static uintptr_t test_beginthreadex(void*    security,
                                    unsigned stack_size,
                                    unsigned(__stdcall* entry)(void*),
                                    void*     argument,
                                    unsigned  flags,
                                    unsigned* id)
{
    if (reject_create()) {
        return 0;
    }
    return _beginthreadex(security, stack_size, entry, argument, flags, id);
}
#    define _beginthreadex test_beginthreadex
#else
static int test_pthread_create(pthread_t*            thread,
                               const pthread_attr_t* attributes,
                               void* (*entry)(void*),
                               void* argument)
{
    if (reject_create()) {
        return EAGAIN;
    }
    return pthread_create(thread, attributes, entry, argument);
}
#    define pthread_create test_pthread_create
#endif

#if OS_WINDOWS || OS_LINUX
static bool fail_cpu_query;
void        thread_test_fail_cpu_query(bool fail) { fail_cpu_query = fail; }
#    if OS_WINDOWS
static BOOL
test_process_affinity(HANDLE process, PDWORD_PTR mask, PDWORD_PTR system)
{
    return fail_cpu_query ? FALSE
                          : GetProcessAffinityMask(process, mask, system);
}
#        define GetProcessAffinityMask test_process_affinity
#    else
#        include <sched.h>
static int test_cpu_affinity(pid_t pid, size_t bytes, cpu_set_t* mask)
{
    if (fail_cpu_query) {
        errno = EACCES;
        return -1;
    }
    return sched_getaffinity(pid, bytes, mask);
}
#        define sched_getaffinity test_cpu_affinity
#    endif
#endif

#include "../../src/core/thread.c"
