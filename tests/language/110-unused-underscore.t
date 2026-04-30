helper :: fn (_unused: i32) -> i32 {
    _scratch := 10
    return 7
}

main :: fn () -> i32 {
    _future := 3
    return helper(1)
}
¬
7
¬

¬
¬
