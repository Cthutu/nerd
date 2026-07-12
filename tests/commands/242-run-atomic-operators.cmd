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
    on value.fetch_add(2, Relaxed) != 46 => return 4
    on value.load(Acquire) != 48 => return 5
    copied : atomic[i32] = value
    on copied.load() != 48 => return 13
    copied = value
    on copied.load() != 48 => return 14

    flag : atomic[bool] = no
    on flag.exchange(yes, AcquireRelease) => return 6
    on !flag.load(Relaxed) => return 7
    on flag.fetch_xor(yes) != yes => return 8
    on flag => return 9
    on flag.compare_exchange(no, yes, AcquireRelease, Acquire) {
        Exchanged => {
        }
        else => return 15
    }

    number := 12
    pointer : atomic[^i32] = ^number
    loaded_pointer : ^i32 = pointer.load(Acquire)
    on loaded_pointer != ^number => return 10
    on pointer.exchange(nil, Release) != ^number => return 11
    on pointer != nil => return 12
    on pointer.compare_exchange_weak(nil, ^number, AcquireRelease, Acquire) {
        Exchanged => {
        }
        else => return 16
    }

    return value - 48
}
¬
0
¬
