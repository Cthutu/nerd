choose :: fn (enabled :: bool = yes) -> i32 {
    return on enabled => 7 else 9
}

main :: fn () -> i32 {
    return choose() + choose(enabled = no) - 16
}
¬
0
¬
