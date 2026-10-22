use std.atomics
Box :: plex { value i32 }
increment :: fn (value: i32) -> i32 { return value + 1 }
main :: fn () {
    a: u32 = 0xf0
    b: u32 = 0x0f
    assert (a ~ b) == 0xff
    assert (a~~b) == (a ~ (~b))
    assert (~a~b) == ((~a) ~ b)
    assert (8 ~ 3 & 1 | 16) == 25
    assert (1 ~ 2 ~ 3) == 0
    w: [4]u32 = [0x12345678, 0xabcdef01, 0xfedcba98, 0x76543210]
    folded := w[0] ~ w[1] ~ w[2] ~ w[3]
    assert folded == 0x317131f1
    rotated := (folded << 1) | (folded >> 31)
    assert rotated == 0x62e263e2
    a~=b
    assert a == 0xff
    a~=~b
    assert a == 0xffffff0f
    stored: atomic[u32] = 0xf0
    stored ~= b
    assert stored == 0xff
    assert stored.fetch_xor(0x0f) == 0xff
    assert stored == 0xf0

    value := 7
    p := ^value
    pp := ^p
    assert p^-1 == 6
    assert p^ -1 == 6
    assert pp^^ == 7
    p^=9
    assert value == 9
    p^~=3
    assert value == 10
    assert (p^~3) == 9
    values := [4, 5, 6]
    ptr := ^values
    assert ptr^[1] == 5
    assert ptr ^[2] == 6
    box := Box { value: 12 }
    pointer := ^box
    assert pointer^.value == 12
    callback: fn (value: i32) -> i32 = increment
    callback_ptr := ^callback
    assert callback_ptr^(41) == 42
    assert callback_ptr^(-1) == 0
}
¬
0
¬

¬

¬
