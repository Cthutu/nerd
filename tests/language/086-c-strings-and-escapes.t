ffi "c" strlen (^u8) -> usize

main :: fn () -> i32 {
    text := "a\0b\tc\n"
    ctext := c"a\0b"
    hex_text := "\x1bA"
    packed := '\x1bA'

    on hex_text.count != 2 => return 1
    on hex_text.data[0] != 27 => return 2
    on hex_text.data[1] != 'A' => return 3
    on packed != ((27 << 8) | 'A') => return 4

    return text.count.as(i32) * 10 + strlen(ctext).as(i32)
}
¬
61
¬

¬

¬
