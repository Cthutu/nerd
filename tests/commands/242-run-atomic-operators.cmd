use std.atomics

main :: fn () -> i32 {
    value : atomic[i32] = 40
    value += 2
    loaded : i32 = value
    value = loaded + 1
    on value.load() != 43 => return 1
    value.store(44)
    on value.exchange(45) != 44 => return 2
    on value.compare_exchange(45, 46) {
        Exchanged => {
        }
        else => return 3
    }
    return value - 46
}
¬
0
¬
