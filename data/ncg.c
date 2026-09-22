/* Helpers used by the C backend, following the nrt allocator contract. */
typedef struct {
    _Alignas(16) void* data;
    uintptr_t count, capacity;
} NcgArray;
static void ncg_reserve(NcgArray**  slot,
                        uintptr_t   capacity,
                        uintptr_t   size,
                        uintptr_t   align,
                        const char* file,
                        uint32_t    line)
{
    if (size && capacity > (SIZE_MAX - sizeof(NcgArray)) / size) {
        abort();
    }
    if (!*slot) {
        *slot =
            nrt_mem_alloc(sizeof(NcgArray) + capacity * size, 16, file, line);
        **slot = (NcgArray){.data = *slot + 1, .capacity = capacity};
        return;
    }
    NcgArray* a = *slot;
    if (capacity <= a->capacity) {
        return;
    }
    a = nrt_mem_realloc(a, sizeof(NcgArray) + capacity * size, 16, file, line);
    a->data     = a + 1;
    a->capacity = capacity;
    *slot       = a;
    (void)align;
}
static void ncg_array_free(NcgArray** slot)
{
    if (*slot) {
        nrt_mem_free(*slot);
        *slot = NULL;
    }
}
