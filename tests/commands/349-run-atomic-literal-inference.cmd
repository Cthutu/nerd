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
delete
¬
