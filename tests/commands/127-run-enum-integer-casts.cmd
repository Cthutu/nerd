Key :: enum {
    None = 0
    Enter = 13
    LowerX = 120
    ArrowUp = 261
}

main :: fn () -> i32 {
    byte : u8 = 'x'
    from_byte := byte.as(Key)
    from_int := 261.as(Key)

    on from_byte != LowerX => return 1
    on from_int != ArrowUp => return 2
    on Key.Enter.as(u8) != 13 => return 3
    on Key.ArrowUp.as(u32) != 261 => return 4
    return 0
}
¬
0
¬

¬
delete
¬
