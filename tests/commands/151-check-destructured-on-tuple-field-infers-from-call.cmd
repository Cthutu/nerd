put :: fn (_ink: u32, _paper: u32) {}

main :: fn () {
    tile: u8 = 1
    (_char, ink, paper) := on tile {
        0x00 => (' ', 0x000000, 0x000000)
        0x01 => (' ', 0xffffff, 0xffffff)
        else => ('?', 0xff00ff, 0x000000)
    }
    put(ink, paper)
}
¬
0
¬

¬
delete
¬

¬
check
¬
