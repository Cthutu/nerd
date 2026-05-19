MemoryStats :: plex #c {
    heap_alloc_count       usize
    heap_realloc_count     usize
    heap_free_count        usize
    heap_bytes_allocated   usize
    heap_bytes_reallocated usize
    heap_bytes_freed       usize
    heap_current_bytes     usize
    heap_peak_bytes        usize
    arena_init_count       usize
    arena_done_count       usize
    arena_alloc_count      usize
    arena_bytes_allocated  usize
    arena_commit_count     usize
    arena_bytes_committed  usize
    array_growth_count     usize
    array_bytes_allocated  usize
}

ffi "nrt" {
    nrt_mem_alloc (size: usize, file_data: ^u8, file_count: usize, line: u32) -> ^void
    nrt_mem_realloc (ptr: ^void, size: usize, file_data: ^u8, file_count: usize, line: u32) -> ^void
    nrt_mem_free (ptr: ^void)
    nrt_mem_size (ptr: ^void) -> usize
    nrt_mem_leak (ptr: ^void)
    nrt_mem_allocation_count () -> usize
    nrt_mem_total_allocated () -> usize
    nrt_mem_stats_snapshot (out: ^MemoryStats)
}

main :: fn () -> i32 {
    file := "memory-test.n"
    before: MemoryStats
    nrt_mem_stats_snapshot(^before)

    ptr := nrt_mem_alloc(8, file.data, file.count, 11)
    on ptr == nil => return 1
    on nrt_mem_size(ptr) != 8 => return 2
    on nrt_mem_allocation_count() != 1 => return 3
    on nrt_mem_total_allocated() != 8 => return 4

    grown := nrt_mem_realloc(ptr, 16, file.data, file.count, 12)
    on nrt_mem_size(grown) != 16 => return 5
    on nrt_mem_total_allocated() != 16 => return 6

    nrt_mem_leak(grown)
    on nrt_mem_allocation_count() != 0 => return 7
    on nrt_mem_total_allocated() != 0 => return 8

    nrt_mem_free(grown)

    after: MemoryStats
    nrt_mem_stats_snapshot(^after)
    on after.heap_alloc_count != before.heap_alloc_count + 1 => return 9
    on after.heap_realloc_count != before.heap_realloc_count + 1 => return 10
    on after.heap_free_count != before.heap_free_count + 1 => return 11
    on after.heap_current_bytes != before.heap_current_bytes => return 12

    return 42
}
¬
42
¬

¬
delete
¬
