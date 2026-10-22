use std.io
Hash :: plex { a u32 b u32 }
impl Display for Hash {
    show :: fn (self: Self) -> string {
        prefix :: "hash:"
        add :: fn (value: u32) -> string {
            return temp_arena.pr($"{value:08X}")
        }
        return temp_arena.pr($"{prefix}{add(self.a)}{add(self.b)}")
    }
}
impl Hash {
    low :: fn (self: Self) -> u32 {
        mask :: 0xFF
        add :: fn (value: u32) -> u32 { return value & 0xFF }
        return add(self.a) & mask
    }
    high :: fn (self: Self) -> u32 {
        add :: fn (value: u32) -> u32 { return value >> 24 }
        return add(self.a)
    }
}
Value :: trait { value :: fn (self: Self) -> u32 }
impl Value for Hash {
    value :: fn (self: Self) -> u32 {
        value :: fn (number: u32) -> u32 { return number + 1 }
        return value(self.b)
    }
}
main :: fn () {
    hash := Hash { a: 0x12345678 b: 0xABCDEF01 }
    prn($"{hash}")
    assert hash.low() == 0x78 && hash.high() == 0x12
    assert hash.value() == 0xABCDEF02
}
¬
0
¬
hash:12345678ABCDEF01
¬

¬
