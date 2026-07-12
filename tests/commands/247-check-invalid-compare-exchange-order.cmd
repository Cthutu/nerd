use std.atomics

main :: fn () {
    value : atomic[i32] = 0
    result := value.compare_exchange(0, 1, Relaxed, Acquire)
    on result {
        Exchanged => {
        }
        else => {
        }
    }
}
¬
1
¬

¬
delete
¬

¬
check
