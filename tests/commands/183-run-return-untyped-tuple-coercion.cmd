choose :: fn (flag: bool) -> (u16, u16) {
    on flag => return (3, 5)
    return (0, 0)
}

main :: fn () -> i32 {
    result := choose(no)
    return (result.0 + result.1).as(i32)
}
¬
0
¬

¬
