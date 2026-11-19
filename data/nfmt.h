// Runtime-owned, locale-independent formatting. No allocator or libc calls.
#ifndef NRT_FORMAT_INCLUDED
#define NRT_FORMAT_INCLUDED
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if defined(__clang__)
#    pragma clang attribute push(__attribute__((no_builtin)),                  \
                                 apply_to = function)
#endif

typedef struct {
    char*  data;
    size_t capacity, count;
    bool   failed;
} NrtFormat;
static void nf_byte(NrtFormat* out, char byte)
{
    if (out->count < out->capacity && out->data != NULL) {
        out->data[out->count] = byte;
    }
    if (out->count == INT_MAX) {
        out->failed = true;
    } else {
        ++out->count;
    }
}
static void nf_repeat(NrtFormat* out, char byte, size_t count)
{
    if (count > INT_MAX - out->count) {
        out->failed = true;
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        nf_byte(out, byte);
    }
}
static void nf_text(NrtFormat* out, const char* text, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        nf_byte(out, text[i]);
    }
}

// Exact binary64 -> decimal coefficient, using base 10^9 arithmetic. The
// largest coefficient (a subnormal times 5^1074) fits in 120 limbs/1100 digits.
static size_t nf_decimal(
    double number, char* digits, int* point, bool* negative, int* special)
{
    union {
        double   f;
        uint64_t u;
    } bits            = {.f = number};
    *negative         = bits.u >> 63;
    unsigned exponent = (unsigned)(bits.u >> 52) & 2047;
    uint64_t mantissa = bits.u & UINT64_C(0xfffffffffffff);
    *special          = exponent == 2047 ? (mantissa ? 2 : 1) : 0;
    if (*special) {
        return 0;
    }
    int power = exponent ? (int)exponent - 1023 - 52 : -1074;
    if (exponent) {
        mantissa |= UINT64_C(1) << 52;
    }
    uint32_t limbs[120];
    size_t   used = 0;
    do {
        limbs[used++] = (uint32_t)(mantissa % 1000000000);
        mantissa /= 1000000000;
    } while (mantissa);
    unsigned multiplier  = power < 0 ? 5 : 2;
    int      repetitions = power < 0 ? -power : power;
    for (int j = 0; j < repetitions; ++j) {
        uint64_t carry = 0;
        for (size_t i = 0; i < used; ++i) {
            uint64_t v = (uint64_t)limbs[i] * multiplier + carry;
            limbs[i]   = (uint32_t)(v % 1000000000);
            carry      = v / 1000000000;
        }
        if (carry) {
            limbs[used++] = (uint32_t)carry;
        }
    }
    size_t   count = 0;
    char     reverse[10];
    size_t   n   = 0;
    uint32_t top = limbs[used - 1];
    do {
        reverse[n++] = (char)('0' + top % 10);
        top /= 10;
    } while (top);
    while (n) {
        digits[count++] = reverse[--n];
    }
    while (--used) {
        uint32_t v = limbs[used - 1];
        for (size_t k = 9; k > 0; --k) {
            reverse[k - 1] = (char)('0' + v % 10);
            v /= 10;
        }
        for (size_t k = 0; k < 9; ++k) {
            digits[count++] = reverse[k];
        }
    }
    *point = (int)count + (power < 0 ? power : 0);
    // Zero's scale is irrelevant and must not become -1073.
    if (count == 1 && digits[0] == '0') {
        *point = 1;
    }
    return count;
}

static size_t nf_round(char* digits, size_t count, int keep, int* point)
{
    if (keep >= (int)count) {
        return count;
    }
    bool up = false;
    if (keep >= 0) {
        char first = digits[keep];
        bool tail  = false;
        for (size_t i = (size_t)keep + 1; i < count; ++i) {
            if (digits[i] != '0') {
                tail = true;
            }
        }
        up = first > '5' ||
             (first == '5' &&
              (tail || (keep > 0 && ((digits[keep - 1] - '0') & 1))));
    }
    if (keep <= 0) {
        digits[0] = up ? '1' : '0';
        *point    = up ? *point - keep + 1 : 1;
        return 1;
    }
    count = (size_t)keep;
    if (up) {
        size_t i = count;
        while (i && digits[i - 1] == '9') {
            digits[--i] = '0';
        }
        if (i) {
            ++digits[i - 1];
        } else {
            digits[0] = '1';
            for (size_t j = 1; j < count; ++j) {
                digits[j] = '0';
            }
            ++*point;
        }
    }
    return count;
}

static void nf_float_body(
    NrtFormat* out, double value, char kind, int precision, bool alternate)
{
    char   digits[1100];
    int    point = 0, special = 0;
    bool   negative;
    size_t count = nf_decimal(value, digits, &point, &negative, &special);
    bool   upper = kind >= 'A' && kind <= 'Z';
    if (upper) {
        kind = (char)(kind + 'a' - 'A');
    }
    if (special) {
        nf_text(out,
                special == 1 ? (upper ? "INF" : "inf")
                             : (upper ? "NAN" : "nan"),
                3);
        return;
    }
    if (precision < 0) {
        precision = 6;
    }
    if (kind == 'g' && precision == 0) {
        precision = 1;
    }
    int keep = kind == 'f'   ? point + precision
               : kind == 'e' ? precision + 1
                             : precision;
    count    = nf_round(digits, count, keep, &point);
    if (kind == 'g') {
        bool exponential = point - 1 < -4 || point - 1 >= precision;
        kind             = exponential ? 'e' : 'f';
        precision        = exponential ? precision - 1 : precision - point;
        if (precision < 0) {
            precision = 0;
        }
        if (!alternate) {
            while (precision > 0) {
                int index = kind == 'e' ? precision : point + precision - 1;
                if (index >= 0 && index < (int)count && digits[index] != '0') {
                    break;
                }
                --precision;
            }
        }
    }
    if (kind == 'e') {
        nf_byte(out, digits[0]);
        if (precision || alternate) {
            nf_byte(out, '.');
        }
        for (int i = 1; i <= precision; ++i) {
            nf_byte(out, i < (int)count ? digits[i] : '0');
        }
        nf_byte(out, upper ? 'E' : 'e');
        int exponent = point - 1;
        nf_byte(out, exponent < 0 ? '-' : '+');
        if (exponent < 0) {
            exponent = -exponent;
        }
        char   reversed[12];
        size_t n = 0;
        do {
            reversed[n++] = (char)('0' + exponent % 10);
            exponent /= 10;
        } while (exponent);
        if (n < 2) {
            nf_byte(out, '0');
        }
        while (n) {
            nf_byte(out, reversed[--n]);
        }
    } else {
        if (point <= 0) {
            nf_byte(out, '0');
        } else {
            for (int i = 0; i < point; ++i) {
                nf_byte(out, i < (int)count ? digits[i] : '0');
            }
        }
        if (precision || alternate) {
            nf_byte(out, '.');
        }
        for (int i = 0; i < precision; ++i) {
            int index = point + i;
            nf_byte(out,
                    index >= 0 && index < (int)count ? digits[index] : '0');
        }
    }
}

static void nf_hex_body(
    NrtFormat* out, double value, char kind, int precision, bool alternate)
{
    union {
        double   f;
        uint64_t u;
    } v        = {.f = value};
    unsigned e = (unsigned)(v.u >> 52) & 2047;
    if (e == 2047) {
        nf_text(out,
                (v.u & UINT64_C(0xfffffffffffff))
                    ? (kind == 'A' ? "NAN" : "nan")
                    : (kind == 'A' ? "INF" : "inf"),
                3);
        return;
    }
    uint64_t mantissa = v.u & UINT64_C(0xfffffffffffff);
    if (e) {
        mantissa |= UINT64_C(1) << 52;
    }
    int exponent = e ? (int)e - 1023 : mantissa ? -1022 : 0;
    int digits   = precision < 0 ? 13 : precision;
    if (digits < 13) {
        unsigned shift     = (unsigned)(13 - digits) * 4;
        uint64_t remainder = mantissa & ((UINT64_C(1) << shift) - 1);
        uint64_t half      = UINT64_C(1) << (shift - 1);
        mantissa >>= shift;
        if (remainder > half || (remainder == half && (mantissa & 1))) {
            ++mantissa;
        }
    } else if (precision < 0) {
        while (digits && (mantissa & 15) == 0) {
            mantissa >>= 4;
            --digits;
        }
    }
    int         stored = digits < 13 ? digits : 13;
    const char* alphabet =
        kind == 'A' ? "0123456789ABCDEF" : "0123456789abcdef";
    nf_byte(out, alphabet[mantissa >> (stored * 4)]);
    if (digits || alternate) {
        nf_byte(out, '.');
    }
    for (int i = 0; i < stored; ++i) {
        nf_byte(out, alphabet[(mantissa >> ((stored - i - 1) * 4)) & 15]);
    }
    if (digits > stored) {
        nf_repeat(out, '0', (size_t)(digits - stored));
    }
    nf_byte(out, kind == 'A' ? 'P' : 'p');
    nf_byte(out, exponent < 0 ? '-' : '+');
    if (exponent < 0) {
        exponent = -exponent;
    }
    char   reverse[12];
    size_t n = 0;
    do {
        reverse[n++] = (char)('0' + exponent % 10);
        exponent /= 10;
    } while (exponent);
    while (n) {
        nf_byte(out, reverse[--n]);
    }
}

static int nrt_vformat(char*       destination,
                       size_t      capacity,
                       const char* format,
                       va_list     original)
{
    NrtFormat out = {destination, capacity ? capacity - 1 : 0, 0, false};
    va_list   args;
    va_copy(args, original);
    while (*format && !out.failed) {
        if (*format != '%') {
            nf_byte(&out, *format++);
            continue;
        }
        ++format;
        bool left = false, plus = false, space = false, zero = false,
             alternate = false;
        for (;;) {
            char f = *format;
            if (f == '-') {
                left = true;
            } else if (f == '+') {
                plus = true;
            } else if (f == ' ') {
                space = true;
            } else if (f == '0') {
                zero = true;
            } else if (f == '#') {
                alternate = true;
            } else {
                break;
            }
            ++format;
        }
        int width = 0, precision = -1;
        if (*format == '*') {
            width = va_arg(args, int);
            ++format;
            if (width < 0) {
                left = true;
                if (width == INT_MIN) {
                    out.failed = true;
                    break;
                }
                width = -width;
            }
        } else {
            while (*format >= '0' && *format <= '9') {
                if (width > (INT_MAX - 9) / 10) {
                    out.failed = true;
                    break;
                }
                width = width * 10 + (*format++ - '0');
            }
        }
        if (*format == '.') {
            ++format;
            precision = 0;
            if (*format == '*') {
                precision = va_arg(args, int);
                ++format;
                if (precision < 0) {
                    precision = -1;
                }
            } else {
                while (*format >= '0' && *format <= '9') {
                    if (precision > (INT_MAX - 9) / 10) {
                        out.failed = true;
                        break;
                    }
                    precision = precision * 10 + (*format++ - '0');
                }
            }
        }
        enum {
            NF_INT,
            NF_CHAR,
            NF_SHORT,
            NF_LONG,
            NF_LLONG,
            NF_SIZE,
            NF_DIFF,
            NF_MAX
        } length = NF_INT;
        if (*format == 'h') {
            ++format;
            length = NF_SHORT;
            if (*format == 'h') {
                ++format;
                length = NF_CHAR;
            }
        } else if (*format == 'l') {
            ++format;
            length = NF_LONG;
            if (*format == 'l') {
                ++format;
                length = NF_LLONG;
            }
        } else if (*format == 'z') {
            ++format;
            length = NF_SIZE;
        } else if (*format == 't') {
            ++format;
            length = NF_DIFF;
        } else if (*format == 'j') {
            ++format;
            length = NF_MAX;
        }
        char kind = *format;
        if (kind) {
            ++format;
        }
        char        buffer[64];
        const char* text         = buffer;
        size_t      count        = 0;
        char        sign         = 0;
        const char* prefix       = "";
        size_t      prefix_count = 0;
        bool        numeric      = false;
        if (kind == 'f' || kind == 'F' || kind == 'e' || kind == 'E' ||
            kind == 'g' || kind == 'G' || kind == 'a' || kind == 'A') {
            if (length != NF_INT && length != NF_LONG) {
                out.failed = true;
                break;
            }
            if (precision > INT_MAX - 1200) {
                out.failed = true;
                break;
            }
            double value = va_arg(args, double);
            union {
                double   f;
                uint64_t u;
            } bits       = {.f = value};
            bool hex     = kind == 'a' || kind == 'A',
                 special = ((bits.u >> 52) & 2047) == 2047;
            if (special) {
                zero = false;
            }
            sign = (bits.u >> 63) ? '-' : plus ? '+' : space ? ' ' : 0;
            NrtFormat measure = {NULL, 0, 0, false};
            if (hex) {
                nf_hex_body(&measure, value, kind, precision, alternate);
            } else {
                nf_float_body(&measure, value, kind, precision, alternate);
            }
            size_t total   = measure.count + (sign ? 1 : 0) +
                             (hex && !special ? 2 : 0),
                   padding = (size_t)width > total ? (size_t)width - total : 0;
            if (!left && !zero) {
                nf_repeat(&out, ' ', padding);
            }
            if (sign) {
                nf_byte(&out, sign);
            }
            if (hex && !special) {
                nf_text(&out, kind == 'A' ? "0X" : "0x", 2);
            }
            if (!left && zero) {
                nf_repeat(&out, '0', padding);
            }
            if (hex) {
                nf_hex_body(&out, value, kind, precision, alternate);
            } else {
                nf_float_body(&out, value, kind, precision, alternate);
            }
            if (left) {
                nf_repeat(&out, ' ', padding);
            }
            continue;
        } else if (kind == 's' && length == NF_INT) {
            text = va_arg(args, const char*);
            if (!text) {
                text = "(null)";
            }
            while ((precision < 0 || count < (size_t)precision) &&
                   text[count]) {
                ++count;
            }
        } else if (kind == 'c' && length == NF_INT) {
            buffer[0] = (char)va_arg(args, int);
            count     = 1;
        } else if (kind == '%') {
            buffer[0] = '%';
            count     = 1;
        } else if (kind == 'd' || kind == 'i' || kind == 'u' || kind == 'o' ||
                   kind == 'x' || kind == 'X' || kind == 'p') {
            numeric            = true;
            bool     is_signed = kind == 'd' || kind == 'i';
            uint64_t value;
            if (kind == 'p') {
                value = (uintptr_t)va_arg(args, void*);
                if (value == 0) {
                    text    = "(nil)";
                    count   = 5;
                    numeric = false;
                    goto emit;
                }
                prefix       = "0x";
                prefix_count = 2;
            } else if (is_signed) {
                int64_t v;
                switch (length) {
                case NF_LONG:
                    v = va_arg(args, long);
                    break;
                case NF_LLONG:
                    v = va_arg(args, long long);
                    break;
                case NF_SIZE:
                case NF_DIFF:
                    v = va_arg(args, ptrdiff_t);
                    break;
                case NF_MAX:
                    v = va_arg(args, intmax_t);
                    break;
                default:
                    v = va_arg(args, int);
                    if (length == NF_CHAR) {
                        v = (signed char)v;
                    }
                    if (length == NF_SHORT) {
                        v = (short)v;
                    }
                    break;
                }
                sign  = v < 0 ? '-' : plus ? '+' : space ? ' ' : 0;
                value = v < 0 ? (uint64_t)0 - (uint64_t)v : (uint64_t)v;
            } else {
                switch (length) {
                case NF_LONG:
                    value = va_arg(args, unsigned long);
                    break;
                case NF_LLONG:
                    value = va_arg(args, unsigned long long);
                    break;
                case NF_SIZE:
                    value = va_arg(args, size_t);
                    break;
                case NF_DIFF:
                    value = va_arg(args, uintptr_t);
                    break;
                case NF_MAX:
                    value = va_arg(args, uintmax_t);
                    break;
                default:
                    value = va_arg(args, unsigned);
                    if (length == NF_CHAR) {
                        value = (unsigned char)value;
                    }
                    if (length == NF_SHORT) {
                        value = (unsigned short)value;
                    }
                    break;
                }
            }
            unsigned base = kind == 'o'                                   ? 8
                            : (kind == 'x' || kind == 'X' || kind == 'p') ? 16
                                                                          : 10;
            const char* alphabet =
                kind == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
            if (alternate && value && (kind == 'x' || kind == 'X')) {
                prefix       = kind == 'X' ? "0X" : "0x";
                prefix_count = 2;
            }
            size_t cursor = sizeof(buffer);
            if (value || precision != 0) {
                do {
                    buffer[--cursor] = alphabet[value % base];
                    value /= base;
                } while (value);
            }
            text  = buffer + cursor;
            count = sizeof(buffer) - cursor;
            if (alternate && kind == 'o' && (count == 0 || text[0] != '0') &&
                precision <= (int)count) {
                precision = (int)count + 1;
            }
        } else if (kind == 'n') {
            switch (length) {
            case NF_CHAR:
                *va_arg(args, signed char*) = (signed char)out.count;
                break;
            case NF_SHORT:
                *va_arg(args, short*) = (short)out.count;
                break;
            case NF_LONG:
                *va_arg(args, long*) = (long)out.count;
                break;
            case NF_LLONG:
                *va_arg(args, long long*) = (long long)out.count;
                break;
            case NF_SIZE:
            case NF_DIFF:
                *va_arg(args, ptrdiff_t*) = (ptrdiff_t)out.count;
                break;
            case NF_MAX:
                *va_arg(args, intmax_t*) = (intmax_t)out.count;
                break;
            default:
                *va_arg(args, int*) = (int)out.count;
                break;
            }
            continue;
        } else {
            out.failed = true;
            break;
        } // Positional, wide and long-double formats are unsupported.
    emit:;
        size_t zeros =
            numeric && precision > (int)count ? (size_t)precision - count : 0;
        size_t total    = count + zeros + prefix_count + (sign ? 1 : 0),
               padding  = (size_t)width > total ? (size_t)width - total : 0;
        bool   zero_pad = numeric && zero && !left && precision < 0;
        if (!left && !zero_pad) {
            nf_repeat(&out, ' ', padding);
        }
        if (sign) {
            nf_byte(&out, sign);
        }
        nf_text(&out, prefix, prefix_count);
        if (zero_pad) {
            nf_repeat(&out, '0', padding);
        }
        nf_repeat(&out, '0', zeros);
        nf_text(&out, text, count);
        if (left) {
            nf_repeat(&out, ' ', padding);
        }
    }
    va_end(args);
    if (capacity && destination) {
        destination[out.count < capacity ? out.count : capacity - 1] = 0;
    }
    return out.failed ? -1 : (int)out.count;
}
static int
nrt_format(char* destination, size_t capacity, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = nrt_vformat(destination, capacity, format, args);
    va_end(args);
    return result;
}
#if defined(__clang__)
#    pragma clang attribute pop
#endif
#endif
