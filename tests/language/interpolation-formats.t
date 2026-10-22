use std.io
constant :: $"{255:02X}/{5:08b}/{64:o}/{-42:06d}"
step :: fn (calls: ^i32) -> u32 { calls^ += 1 return calls^.as(u32) }
main :: fn () {
    word: u32 = 0x0123ABCD
    prn($"{word >> 24 & 0xFF:02X}{word >> 16 & 0xFF:02X}{word >> 8 & 0xFF:02X}{word & 0xFF:02X}")
    prn(constant)
    small: i8 = -128
    large: u64 = 0xffffffffffffffff
    min: i64 = -9223372036854775807 - 1
    prn($"[{small:6d}] [{small:06d}] {small:x} {large:X} {min:d}")
    zero: u8
    prn($"{zero:02X}/{large:02x}/{word:12}")
    calls: i32
    prn($"{step(^calls):02X}{step(^calls):02X}{$"{step(^calls):02X}"}{step(^calls):02X}")
    assert calls == 4
    assert $"{${ local: u32 = 10 break local }:02X}" == "0A"
    assert $"{(fn (n: u32) => n)(word):08X}" == "0123ABCD"
    assert $"{on yes => word else 0:08X}" == "0123ABCD"
    label := "plain"
    prn($"{label}/{word:08x}" +"/{word:08X}")
    prn(temp_arena.pr($"{word:08X}"))
    padded := $"{zero:1024d}"
    assert padded.count == 1024 && padded[0] == ' ' && padded[1023] == '0'
    prn("integer interpolation formats passed")
}
¬
0
¬
0123ABCD
FF/00000101/100/-00042
[  -128] [-00128] -80 FFFFFFFFFFFFFFFF -9223372036854775808
00/ffffffffffffffff/    19114957
01020304
plain/0123abcd/0123ABCD
0123ABCD
integer interpolation formats passed
¬

¬
