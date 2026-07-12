use std.atomics

main :: fn () -> i32 {
    value : atomic[i32] = 1
    value.store(2, Relaxed)
    old := value.exchange(3, AcquireRelease)
    fetched := value.fetch_add(4, Release)
    result := value.compare_exchange(7,
                                     8,
                                     SequentiallyConsistent,
                                     Acquire)
    on result {
        Exchanged => return old + fetched
        else => return 1
    }
}
¬
