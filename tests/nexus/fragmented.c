/* Linux-only deterministic short I/O and EAGAIN injection. All accepted bytes
 * still go through the real socket, so wire duplication/corruption is
 * observable. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t send(int fd, const void* bytes, size_t length, int flags)
{
    static ssize_t (*real_send)(int, const void*, size_t, int);
    static unsigned calls;
    if (!real_send) {
        real_send = dlsym(RTLD_NEXT, "send");
    }
    /* Stall the first client's text while allowing negotiation and other peers.
     * Its pending message must not stop other replies, and shutdown cancels it.
     */
    if (getenv("NEXUS_TEST_TEXT_SEND_WOULD_BLOCK") && length != 0 &&
        ((const unsigned char*)bytes)[0] >= 32 &&
        ((const unsigned char*)bytes)[0] < 127) {
        static int blocked_fd = -1;
        if (blocked_fd == -1) {
            blocked_fd                = fd;
            static const char ready[] = "Nexus text send blocked\n";
            (void)write(STDERR_FILENO, ready, sizeof(ready) - 1);
        }
        if (fd == blocked_fd) {
            errno = EAGAIN;
            return -1;
        }
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
    /* Per-fd cadence avoids starving one peer forever in a seven-peer scan. */
    static unsigned calls[1024];
    if (!real_recv) {
        real_recv = dlsym(RTLD_NEXT, "recv");
    }
    if (getenv("NEXUS_TEST_RECV_WOULD_BLOCK") ||
        (fd >= 0 && fd < 1024 && ++calls[fd] % 7 == 0)) {
        errno = EAGAIN;
        return -1;
    }
    size_t cap = length >= 4096 ? 4096 : 1;
    return real_recv(fd, bytes, length < cap ? length : cap, flags);
}

/* Optional deterministic refused attempts complement the real delayed listener.
 * A scheduler pause cannot let a broken no-retry connect accidentally pass. */
int connect(int fd, const struct sockaddr* address, socklen_t length)
{
    static int (*real_connect)(int, const struct sockaddr*, socklen_t);
    static unsigned calls;
    if (!real_connect) {
        real_connect = dlsym(RTLD_NEXT, "connect");
    }
    const char* count = getenv("NEXUS_TEST_CONNECT_REFUSALS");
    if (count && ++calls <= strtoul(count, NULL, 10)) {
        errno = ECONNREFUSED;
        return -1;
    }
    return real_connect(fd, address, length);
}

/* UDP preserves message boundaries, but readiness can still become stale. */
ssize_t recvfrom(int              fd,
                 void*            bytes,
                 size_t           length,
                 int              flags,
                 struct sockaddr* address,
                 socklen_t*       address_length)
{
    static ssize_t (*real_recvfrom)(
        int, void*, size_t, int, struct sockaddr*, socklen_t*);
    if (!real_recvfrom) {
        real_recvfrom = dlsym(RTLD_NEXT, "recvfrom");
    }
    if (getenv("NEXUS_TEST_RECV_WOULD_BLOCK")) {
        errno = EAGAIN;
        return -1;
    }
    return real_recvfrom(fd, bytes, length, flags, address, address_length);
}

ssize_t sendto(int                    fd,
               const void*            bytes,
               size_t                 length,
               int                    flags,
               const struct sockaddr* address,
               socklen_t              address_length)
{
    static ssize_t (*real_sendto)(
        int, const void*, size_t, int, const struct sockaddr*, socklen_t);
    if (!real_sendto) {
        real_sendto = dlsym(RTLD_NEXT, "sendto");
    }
    if (getenv("NEXUS_TEST_SEND_WOULD_BLOCK")) {
        errno = EAGAIN;
        return -1;
    }
    return real_sendto(fd, bytes, length, flags, address, address_length);
}
