#define _GNU_SOURCE 1
#include "../../data/nfmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static void compare(const char* format, ...)
{
    char    actual[4096], expected[4096];
    va_list args, copy;
    va_start(args, format);
    va_copy(copy, args);
    int a = nrt_vformat(actual, sizeof(actual), format, args);
    int e = vsnprintf(expected, sizeof(expected), format, copy);
    va_end(copy);
    va_end(args);
    if (a != e || strcmp(actual, expected)) {
        fprintf(stderr,
                "format %s: got [%s] (%d), expected [%s] (%d)\n",
                format,
                actual,
                a,
                expected,
                e);
        exit(1);
    }
}
int main(void)
{
    uint64_t state = 1;
    for (int i = 0; i < 3000; ++i) {
        state = state * UINT64_C(6364136223846793005) + 1;
        union {
            uint64_t u;
            double   f;
        } value = {.u = state};
        if (((state >> 52) & 2047) == 2047) {
            continue;
        }
        compare("%g", value.f);
        compare("%.17g", value.f);
        compare("%.0f", value.f);
        compare("%+.6e", value.f);
        compare("%#.9g", value.f);
        compare("%020.4f", value.f);
        compare("%a", value.f);
        compare("%020.3A", value.f);
    }
    const double values[] = {0.,
                             -0.,
                             0.5,
                             1.5,
                             2.5,
                             9.99,
                             0.06,
                             0.005,
                             1e-4,
                             1e-5,
                             999999.5,
                             1e20,
                             0x1p-1074,
                             0x1.fffffffffffffp1023};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        for (int p = 0; p < 20; ++p) {
            compare("%*.*f", 30, p, values[i]);
            compare("%.*g", p, values[i]);
            compare("%.*a", p, values[i]);
        }
    }
    compare("%d %u %lld %llu %zu %td",
            INT_MIN,
            UINT_MAX,
            LLONG_MIN,
            ULLONG_MAX,
            SIZE_MAX,
            PTRDIFF_MIN);
    compare("%#x %#o %.0d %08x %hhd %hu", 42u, 0u, 0, 42u, 255, 65535);
    compare("%*.*f %s %d", 8, 2, 1.25, "ok", 42);
    union {
        uint64_t u;
        double   f;
    } infinity = {.u = UINT64_C(0x7ff0000000000000)},
      nan      = {.u = UINT64_C(0x7ff8000000000000)};
    compare("%+012f %A", infinity.f, nan.f);
    char counts[16];
    int  observed = -1;
    if (nrt_format(counts, sizeof(counts), "abc%nxyz", &observed) != 6 ||
        observed != 3) {
        return 1;
    }
    char buffer[4];
    int  n = nrt_format(buffer, sizeof(buffer), "%s", "abcdef");
    if (n != 6 || strcmp(buffer, "abc")) {
        return 1;
    }
    long page = sysconf(_SC_PAGESIZE);
    if (page <= 0) {
        return 1;
    }
    char* mapping = mmap(NULL,
                         (size_t)page * 2,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1,
                         0);
    if (mapping == MAP_FAILED ||
        mprotect(mapping + page, (size_t)page, PROT_NONE) != 0) {
        return 1;
    }
    mapping[page - 3] = 'a';
    mapping[page - 2] = 'b';
    mapping[page - 1] = 'c';
    compare("%.3s", mapping + page - 3);
    // Check our zero-length path directly: ASan's printf interceptor itself
    // probes strlen for %.0s even though the real host formatter does not.
    if (nrt_format(buffer, sizeof(buffer), "%.0s", mapping + page) != 0 ||
        buffer[0] != 0) {
        return 1;
    }
    munmap(mapping, (size_t)page * 2);
    puts("runtime format passed");
    return 0;
}
