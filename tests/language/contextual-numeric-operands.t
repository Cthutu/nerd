-- Concrete siblings must constrain nested literal arithmetic before lowering.
main :: fn () {
    bytes : [2000]u8
    index := 3.as(usize)
    bytes[22 * 80 + index] = 7
    assert bytes[1763] == 7
    wide := 3.as(u64)
    assert 65536 * 65536 + wide == 4294967299
    assert wide + (65536 * 65536) == 4294967299
    assert 65536 * 65536 < wide + 4294967296
    assert wide + 4294967296 > 65536 * 65536
    signed := 3.as(i64)
    assert (-20 * 80) + signed == -1597
    small := 3.as(u8)
    assert (2 * 8) + small == 19
    real := 0.5.as(f32)
    assert (2 * 8) + real == 16.5
    assert real + (2 * 8) == 16.5
    prn($"numeric operands {bytes[1763]}")
}
¬
0
¬
numeric operands 7

¬
¬
