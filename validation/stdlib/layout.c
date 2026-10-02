#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t tag;
    uintptr_t counter;
    uint8_t tail;
} CRecord;

int main(void)
{
    printf("C layout: %zu %zu %zu\n", sizeof(CRecord),
           offsetof(CRecord, counter), offsetof(CRecord, tail));
    return 0;
}
