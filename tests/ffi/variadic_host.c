#include <assert.h>
#include <stdint.h>
#include <stdio.h>

extern int     string_eq(int a, int b);
extern int     string_builder_reset(int a, int b);
extern int     va_imported(int fixed, ...);
extern int64_t va_integers(int count, ...);
extern double  va_doubles(int count, ...);
extern int     va_mixed(int marker, const char* expected, ...);
extern int     va_copies(int mode, ...);
extern int     va_format(const char* format, ...);

int main(void)
{
    assert(string_eq(20, 22) == 42);
    assert(string_builder_reset(20, 22) == 42);
    assert(va_imported(0, 42) == 42);
    assert(va_integers(0) == 0);
    assert(va_integers(12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12) == 78);
    assert(va_doubles(12, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.) ==
           78.);
    const char* text = "ok";
    assert(va_mixed(7,
                    text,
                    (signed char)-3,
                    (unsigned short)65000,
                    (float)1.25,
                    text,
                    INT64_C(-9000000000),
                    UINT64_C(18000000000)) == 42);
    for (int i = 0; i < 100; ++i) {
        assert(va_copies(0, 20, 22) == 42);
        assert(va_copies(1, 20, 22) == 42);
    }
    assert(va_format("%*.*f %s %d", 8, 2, 1.25, "ok", 42) == 42);
    puts("42");
    return 0;
}
