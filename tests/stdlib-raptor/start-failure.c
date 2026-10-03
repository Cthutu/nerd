/* Native Linux failure injection; no library test hooks or public API changes.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned attempts, started, joined;

int pthread_create(pthread_t*            thread,
                   const pthread_attr_t* attributes,
                   void* (*entry)(void*),
                   void* context)
{
    typedef int (*create_fn)(
        pthread_t*, const pthread_attr_t*, void* (*)(void*), void*);
    create_fn native = (create_fn)dlsym(RTLD_NEXT, "pthread_create");
    if (++attempts == 3) {
        return EAGAIN;
    }
    int result = native(thread, attributes, entry, context);
    if (!result) {
        ++started;
    }
    return result;
}

int pthread_join(pthread_t thread, void** result)
{
    typedef int (*join_fn)(pthread_t, void**);
    join_fn native = (join_fn)dlsym(RTLD_NEXT, "pthread_join");
    int     status = native(thread, result);
    if (!status) {
        ++joined;
    }
    return status;
}

__attribute__((destructor)) static void verify_cleanup(void)
{
    if (attempts != 5 || started != 4 || joined != started) {
        fprintf(stderr,
                "partial startup leaked: attempts=%u started=%u joined=%u\n",
                attempts,
                started,
                joined);
        abort();
    }
}
