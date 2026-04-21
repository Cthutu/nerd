#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;

#define DEF_SLICE(type)                                                        \
    typedef struct {                                                           \
        type*  data;                                                           \
        size_t count;                                                          \
    }

DEF_SLICE(u8) string;

void pr(string str) { fwrite(str.data, 1, str.count, stdout); }

void prn(string str)
{
    fwrite(str.data, 1, str.count, stdout);
    fputc('\n', stdout);
}
