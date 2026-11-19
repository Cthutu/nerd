#define NRT_FREESTANDING 1
#include "../../data/nmath.h"
#include <dlfcn.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
static void
check(double actual, double expected, const char* name, double input)
{
    if (isnan(actual) && isnan(expected)) {
        return;
    }
    if (actual == expected && signbit(actual) == signbit(expected)) {
        return;
    }
    if ((actual != 0 || expected != 0) && isfinite(actual) &&
        isfinite(expected) &&
        fabs(actual - expected) <= 8e-15 * fmax(fabs(expected), 0x1p-1022)) {
        return;
    }
    fprintf(
        stderr, "%s(%a): got %a, expected %a\n", name, input, actual, expected);
    exit(1);
}
int main(void)
{
    void* library = dlopen("libm.so.6", RTLD_NOW | RTLD_LOCAL);
    if (!library) {
        return 1;
    }
    const char* names[]        = {"sin", "cos", "tan", "asin", "acos", "sqrt"};
    double (*native[])(double) = {sin, cos, tan, asin, acos, sqrt};
    uint64_t state             = 1;
    for (int function = 0; function < 6; ++function) {
        double (*oracle)(double) =
            (double (*)(double))dlsym(library, names[function]);
        if (!oracle) {
            return 1;
        }
        for (int i = 0; i < 12000; ++i) {
            state = state * UINT64_C(6364136223846793005) + 1;
            union {
                uint64_t u;
                double   f;
            } v = {.u = state};
            check(native[function](v.f), oracle(v.f), names[function], v.f);
        }
        double edges[] = {0.,
                          -0.,
                          INFINITY,
                          -INFINITY,
                          NAN,
                          1.,
                          -1.,
                          0x1.921fb54442d18p0,
                          0x1.921fb54442d18p1,
                          0x1.fffffffffffffp1023,
                          0x1p-1074};
        for (unsigned i = 0; i < sizeof(edges) / sizeof(edges[0]); ++i) {
            check(native[function](edges[i]),
                  oracle(edges[i]),
                  names[function],
                  edges[i]);
        }
    }
    double (*oracle_atan2)(double, double) =
        (double (*)(double, double))dlsym(library, "atan2");
    const double edges[] = {
        0., -0., 1., -1., INFINITY, -INFINITY, NAN, 1e-300, 1e300};
    for (unsigned i = 0; i < sizeof(edges) / sizeof(edges[0]); ++i) {
        for (unsigned j = 0; j < sizeof(edges) / sizeof(edges[0]); ++j) {
            check(atan2(edges[i], edges[j]),
                  oracle_atan2(edges[i], edges[j]),
                  "atan2",
                  edges[i]);
        }
    }
    puts("runtime maths passed");
    return 0;
}
