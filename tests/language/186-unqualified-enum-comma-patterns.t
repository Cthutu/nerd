Key :: enum {
    Escape = 27
    Q      = 81
    Other  = 99
}

score :: fn (key: Key) -> i32 {
    return on key {
        Escape, Q => 1
        Other => 2
    }
}

block_score :: fn (key: Key) -> i32 {
    return on key {
        Escape, Q => {
            break 3
        }
        Other => 4
    }
}

main :: fn () -> i32 {
    return score(Q) + block_score(Escape)
}
¬
4
¬

¬

¬
