=== case: 242-run-atomic-operators ===
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
    on value.fetch_add(2, order = Relaxed) != 46 => return 4
    on value.load(order = Acquire) != 48 => return 5
    copied : atomic[i32] = value
    on copied.load() != 48 => return 13
    copied = value
    on copied.load() != 48 => return 14

    flag : atomic[bool] = no
    on flag.exchange(yes, order = AcquireRelease) => return 6
    on !flag.load(order = Relaxed) => return 7
    on flag.fetch_xor(yes) != yes => return 8
    on flag => return 9
    on flag.compare_exchange(no, yes, success_order = AcquireRelease, failure_order = Acquire) {
        Exchanged => {
        }
        else => return 15
    }

    number := 12
    pointer : atomic[^i32] = ^number
    loaded_pointer : ^i32 = pointer.load(order = Acquire)
    on loaded_pointer != ^number => return 10
    on pointer.exchange(nil, order = Release) != ^number => return 11
    on pointer != nil => return 12
    on pointer.compare_exchange_weak(nil, ^number, success_order = AcquireRelease, failure_order = Acquire) {
        Exchanged => {
        }
        else => return 16
    }

    return value - 48
}
¬
0
¬

¬

¬

=== case: 251-run-atomic-method-name-is-not-intrinsic ===
impl atomic[i32] {
    load :: fn (_self: ^atomic[i32]) -> i32 {
        return 73
    }
}

main :: fn () -> i32 {
    value : atomic[i32] = 0
    return value.load() - 73
}
¬
0
¬

¬

¬

=== case: 349-run-atomic-literal-inference ===
use std.atomics

initial :: 7
State :: plex { count atomic[usize] }
main :: fn () {
    count: atomic[usize] = 7
    named: atomic[usize] = initial
    expression: atomic[u64] = (3 + 4) * 2
    wide: atomic[u64] = 0xffff_ffff_ffff_ffff
    small: atomic[u8] = 255
    negative: atomic[i8] = -128
    magnitude :: 128
    named_min: i8 = -magnitude
    min64: i64 = -9223372036854775808
    max64: u64 = 18446744073709551615
    max32: u32 = 4294967295
    narrowed: u8 = 256.as(u8)
    assert named_min == -128 && min64 == -9223372036854775808
    assert max64 == 18446744073709551615 && max32 == 4294967295
    assert narrowed == 0
    state := State { count: 7 }
    assert count.load() == 7 && named.load() == 7
    assert expression.load() == 14 && wide.load() == 0xffff_ffff_ffff_ffff
    assert small.load() == 255 && negative.load() == -128
    assert state.count.load() == 7
    count = 9
    count += 2
    assert count.load() == 11
    prn("Atomic literal inference passed")
}
¬
0
¬
Atomic literal inference passed

¬

¬
