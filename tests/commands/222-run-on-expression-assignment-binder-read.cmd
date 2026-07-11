out : u8

maybe :: fn () -> ?u8 {
    return 7
}

main :: fn () -> i32 {
    out = on maybe() {
        program => program
        else => 0
    }
    prn($"{out}")
    return out.as(i32)
}
¬
7
¬
7

¬
delete
¬
