/* Linux-only deterministic short I/O and EAGAIN injection. All accepted bytes
 * still go through the real socket, so wire duplication/corruption is
 * observable. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>

ssize_t send(int fd, const void* bytes, size_t length, int flags)
{
    static ssize_t (*real_send)(int, const void*, size_t, int);
    static unsigned calls;
    if (!real_send) {
        real_send = dlsym(RTLD_NEXT, "send");
    }
    if (++calls % 7 == 0) {
        errno = EAGAIN;
        return -1;
    }
    size_t cap = length >= 4096 ? 4096 : 2;
    return real_send(fd, bytes, length < cap ? length : cap, flags);
}

ssize_t recv(int fd, void* bytes, size_t length, int flags)
{
    static ssize_t (*real_recv)(int, void*, size_t, int);
    static unsigned calls;
    if (!real_recv) {
        real_recv = dlsym(RTLD_NEXT, "recv");
    }
    if (++calls % 7 == 0) {
        errno = EAGAIN;
        return -1;
    }
    size_t cap = length >= 4096 ? 4096 : 1;
    return real_recv(fd, bytes, length < cap ? length : cap, flags);
}
