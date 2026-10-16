/* Verify the opaque layout assumptions before executing the Nerd bindings. */
#include <stdint.h>
#if defined(_WIN32)
#    include <windows.h>
_Static_assert(sizeof(SRWLOCK) == sizeof(uintptr_t), "SRWLOCK size");
_Static_assert(_Alignof(SRWLOCK) == _Alignof(uintptr_t), "SRWLOCK alignment");
_Static_assert(sizeof(CONDITION_VARIABLE) == sizeof(uintptr_t),
               "condition size");
_Static_assert(_Alignof(CONDITION_VARIABLE) == _Alignof(uintptr_t),
               "condition alignment");
#else
#    include <pthread.h>
#    if !defined(__linux__) || !defined(__x86_64__) || !defined(__GLIBC__)
#        error "This foundation currently targets Linux x86-64 glibc only"
#    endif
_Static_assert(sizeof(pthread_t) == sizeof(uintptr_t), "pthread_t size");
_Static_assert(sizeof(pthread_mutex_t) == 5 * sizeof(uintptr_t), "mutex size");
_Static_assert(_Alignof(pthread_mutex_t) == _Alignof(uintptr_t),
               "mutex alignment");
_Static_assert(sizeof(pthread_cond_t) == 6 * sizeof(uintptr_t),
               "condition size");
_Static_assert(_Alignof(pthread_cond_t) == _Alignof(uintptr_t),
               "condition alignment");
#endif
int main(void) { return 0; }
