// Deterministic Linux select interruption/early-timeout and connect retry
// probe.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>

typedef int (*select_fn)(int, fd_set*, fd_set*, fd_set*, struct timeval*);

int select(int             count,
           fd_set*         reads,
           fd_set*         writes,
           fd_set*         exceptions,
           struct timeval* timeout)
{
    static unsigned injected;
    static long     previous;
    static int      connect_timeout;
    select_fn       native = (select_fn)dlsym(RTLD_NEXT, "select");
    if (!native) {
        errno = EINVAL;
        return -1;
    }
    if (!writes && injected < 5) {
        if (!timeout || timeout->tv_sec != 0) {
            errno = EINVAL;
            return -1;
        }
        long remaining = timeout->tv_usec;
        if (injected && remaining > previous - 1000) {
            // A restarted relative timeout is a contract violation.
            errno = EINVAL;
            return -1;
        }
        previous              = remaining;
        struct timespec delay = {0, 5000000};
        nanosleep(&delay, NULL);
        ++injected;
        // Also simulate native returns before the absolute deadline expires.
        if (injected > 3) {
            return 0;
        }
        errno = EINTR;
        return -1;
    }
    if (writes && !connect_timeout) {
        connect_timeout = 1;
        FD_ZERO(writes);
        if (exceptions) {
            FD_ZERO(exceptions);
        }
        return 0;
    }
    return native(count, reads, writes, exceptions, timeout);
}
