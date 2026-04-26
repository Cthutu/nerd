ffi "c" strlen (^u8) -> usize

main :: fn () -> i32 {
    text := "a\0b\tc\n"
    ctext := c"a\0b"

    return text.count.as(i32) * 10 + strlen(ctext).as(i32)
}
¬
61
¬

¬

¬

