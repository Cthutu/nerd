choose_with_else :: fn () -> i32 {
    value: i32 = undefined
    on yes => value = 21 else value = 3
    return value
}

choose_bool :: fn (flag: bool) -> i32 {
    value: i32 = undefined
    on flag {
        yes => value = 2
        else => value = 4
    }
    return value
}

main :: fn () -> i32 {
    return choose_with_else() + choose_bool(yes) + choose_bool(no)
}
¬
27
¬

¬
¬
