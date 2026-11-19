// Locale/errno-independent binary64 maths for Linux executables.
#ifndef NRT_MATH_INCLUDED
#define NRT_MATH_INCLUDED
#if defined(NRT_FREESTANDING)
#    include <stdint.h>
#    pragma GCC visibility push(hidden)
static double nm_abs(double x)
{
    union {
        double   f;
        uint64_t u;
    } v = {.f = x};
    v.u &= UINT64_C(0x7fffffffffffffff);
    return v.f;
}
static int nm_sign(double x)
{
    union {
        double   f;
        uint64_t u;
    } v = {.f = x};
    return (int)(v.u >> 63);
}
static double nm_nan(void)
{
    union {
        double   f;
        uint64_t u;
    } v = {.u = UINT64_C(0x7ff8000000000000)};
    return v.f;
}
static int nm_bit(const uint32_t* words, int bit)
{
    return (int)((words[bit / 32] >> (bit % 32)) & 1);
}
// Payne-Hanek reduction. floor(2/pi * 2^1280), least-significant limb first.
// Derived from Chudnovsky's pi series with 500 decimal digits of precision.
static double nm_reduce(double x, int* quadrant)
{
    static const uint32_t inverse[40] = {
        0x9af4361d, 0xf0cfbc20, 0xfc7b6bab, 0x56033046, 0x1f8d5d08, 0x6bfb5fb1,
        0x8a5292ea, 0x3d0739f7, 0xebe5f17b, 0x7527bac7, 0x9e5fea2d, 0x4f463f66,
        0x27cb09b7, 0x6d367ecf, 0x5a0a6d1f, 0xef2f118b, 0xde05980f, 0x1ff897ff,
        0xbdf9283b, 0x9c845f8b, 0x835339f4, 0x3991d639, 0xb45f7e41, 0xe99c7026,
        0x2ebb4484, 0xe88235f5, 0xb129a73e, 0xfe1deb1c, 0x09d1921c, 0x06492eea,
        0x424dd2e0, 0xb7246e3a, 0xdebbc561, 0xfe5163ab, 0x3c439041, 0xdb629599,
        0xf534ddc0, 0xfc2757d1, 0x4e441529, 0xa2f9836e};
    *quadrant = 0;
    if (nm_abs(x) <= 0.78539816339744830962) {
        return x;
    }
    union {
        double   f;
        uint64_t u;
    } v          = {.f = x};
    int exponent = (int)((v.u >> 52) & 2047);
    if (exponent == 2047) {
        return nm_nan();
    }
    uint64_t mantissa = (v.u & UINT64_C(0xfffffffffffff)) | (UINT64_C(1) << 52);
    uint32_t product[42];
    for (int i = 0; i < 42; ++i) {
        product[i] = 0;
    }
    uint64_t carry = 0;
    for (int i = 0; i < 40; ++i) {
        uint64_t n = (uint64_t)inverse[i] * (uint32_t)mantissa + carry;
        product[i] = (uint32_t)n;
        carry      = n >> 32;
    }
    product[40] = (uint32_t)carry;
    carry       = 0;
    for (int i = 0; i < 40; ++i) {
        uint64_t n =
            (uint64_t)inverse[i] * (mantissa >> 32) + product[i + 1] + carry;
        product[i + 1] = (uint32_t)n;
        carry          = n >> 32;
    }
    product[41]     = (uint32_t)carry;
    int    shift    = 1280 - (exponent - 1023 - 52);
    int    q        = nm_bit(product, shift) + 2 * nm_bit(product, shift + 1);
    int    round    = nm_bit(product, shift - 1);
    double fraction = 0, weight = 0.5;
    for (int i = 1; i <= 128; ++i) {
        int bit = nm_bit(product, shift - i);
        if (round) {
            bit = 1 - bit;
        }
        if (bit) {
            fraction += weight;
        }
        weight *= 0.5;
    }
    if (round) {
        fraction = -fraction;
        q        = (q + 1) & 3;
    }
    if (nm_sign(x)) {
        fraction = -fraction;
        q        = (-q) & 3;
    }
    *quadrant = q;
    return fraction * 1.57079632679489655800 +
           fraction * 6.12323399573676603587e-17;
}
static double nm_sine(double x)
{
    if (x == 0) {
        return x;
    }
    double square = x * x;
    double term = x, sum = x;
    for (int n = 1; n <= 9; ++n) {
        term *= -square / ((2 * n) * (2 * n + 1));
        sum += term;
    }
    return sum;
}
static double nm_cosine(double x)
{
    double square = x * x, term = 1, sum = 1;
    for (int n = 1; n <= 9; ++n) {
        term *= -square / ((2 * n - 1) * (2 * n));
        sum += term;
    }
    return sum;
}
double sin(double x)
{
    int    q;
    double r = nm_reduce(x, &q);
    switch (q) {
    case 0:
        return nm_sine(r);
    case 1:
        return nm_cosine(r);
    case 2:
        return -nm_sine(r);
    default:
        return -nm_cosine(r);
    }
}
double cos(double x)
{
    int    q;
    double r = nm_reduce(x, &q);
    switch (q) {
    case 0:
        return nm_cosine(r);
    case 1:
        return -nm_sine(r);
    case 2:
        return -nm_cosine(r);
    default:
        return nm_sine(r);
    }
}
double tan(double x)
{
    int    q;
    double r = nm_reduce(x, &q);
    return q & 1 ? -nm_cosine(r) / nm_sine(r) : nm_sine(r) / nm_cosine(r);
}
double sqrt(double x)
{
    double result;
#    if defined(__x86_64__)
    __asm__("sqrtsd %1,%0" : "=x"(result) : "x"(x));
#    else
    __asm__("fsqrt %d0,%d1" : "=w"(result) : "w"(x));
#    endif
    return result;
}
static double nm_atan(double x)
{
    int invert = x > 1;
    if (invert) {
        x = 1 / x;
    }
    int shifted = x > 0.4142135623730950488;
    if (shifted) {
        x = (x - 1) / (x + 1);
    }
    double square = x * x, term = x, sum = x;
    for (int n = 1; n <= 23; ++n) {
        term *= -square;
        sum += term / (2 * n + 1);
    }
    if (shifted) {
        sum += 0.78539816339744830962;
    }
    if (invert) {
        sum = 1.57079632679489661923 - sum;
    }
    return sum;
}
double atan2(double y, double x)
{
    const double pi = 3.14159265358979323846, half = 1.57079632679489661923;
    if (x != x || y != y) {
        return x + y;
    }
    double ax = nm_abs(x), ay = nm_abs(y);
    double angle;
    if (y == 0) {
        angle = nm_sign(x) ? pi : 0;
    } else if (x == 0) {
        angle = half;
    } else if (ay > 0x1.fffffffffffffp1023) {
        angle = ax > 0x1.fffffffffffffp1023 ? (nm_sign(x) ? 3 * pi / 4 : pi / 4)
                                            : half;
    } else if (ax > 0x1.fffffffffffffp1023) {
        angle = nm_sign(x) ? pi : 0;
    } else {
        angle = nm_atan(ay / ax);
        if (nm_sign(x)) {
            angle = pi - angle;
        }
    }
    return nm_sign(y) ? -angle : angle;
}
double asin(double x)
{
    if (nm_abs(x) > 1) {
        return nm_nan();
    }
    return atan2(x, sqrt((1 - x) * (1 + x)));
}
double acos(double x)
{
    if (nm_abs(x) > 1) {
        return nm_nan();
    }
    return atan2(sqrt((1 - x) * (1 + x)), x);
}
#    pragma GCC visibility pop
#endif
#endif
