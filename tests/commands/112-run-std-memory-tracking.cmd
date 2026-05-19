use std.mem

main :: fn () -> i32 {
    before := stats_snapshot()

    bytes := alloc(8)
    on bytes.data == nil => return 1
    on size(bytes) != 8 => return 2
    on allocation_count() != 1 => return 3
    on total_allocated() != 8 => return 4

    bytes.data[0] = 42
    grown := realloc(bytes, 16)
    on size(grown) != 16 => return 5
    on total_allocated() != 16 => return 6
    on grown.data[0] != 42 => return 7

    leak(grown)
    on allocation_count() != 0 => return 8
    on total_allocated() != 0 => return 9

    free(grown)

    after := stats_snapshot()
    on after.heap_alloc_count != before.heap_alloc_count + 1 => return 10
    on after.heap_realloc_count != before.heap_realloc_count + 1 => return 11
    on after.heap_free_count != before.heap_free_count + 1 => return 12
    on after.heap_current_bytes != before.heap_current_bytes => return 13

    return 42
}
¬
42
¬

¬
delete
¬
