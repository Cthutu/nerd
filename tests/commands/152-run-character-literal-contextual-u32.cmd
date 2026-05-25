put :: fn (_ch: u32, _ink: u32, _paper: u32) {}

main :: fn () -> i32 {
    tile: u8 = 1
    (char, ink, paper) := on tile {
        0x00 => (' ', 0x000000, 0x000000)
        0x01 => (' ', 0xffffff, 0xffffff)
        else => ('?', 0xff00ff, 0x000000)
    }
    put(char, ink, paper)
    on '╭' != 9581 => return 1
    return 0
}
¬
0
¬

¬
delete
¬
--llvm
