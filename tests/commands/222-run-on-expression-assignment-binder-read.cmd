out : u8

maybe :: fn () -> Option[u8] {
    return Some(7)
}

main :: fn () -> i32 {
    out = on maybe() {
        Some(program) => program
        None => 0
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
