main :: fn () {
    (char, ink, paper) := on tile { 0x00 => (' ', 0x000000, 0x000000) 0x01 => (' ', 0xffffff, 0xffffff) else => ('?', 0xff00ff, 0x000000) }
}
¬
main :: fn () {
    (char, ink, paper) := on tile {
        0x00  => (' ', 0x000000, 0x000000)
        0x01  => (' ', 0xffffff, 0xffffff)
        else  => ('?', 0xff00ff, 0x000000)
    }
}
