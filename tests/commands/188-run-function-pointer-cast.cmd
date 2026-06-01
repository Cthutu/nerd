Callback :: fn () -> i32

answer :: fn () -> i32 {
    return 42
}

main :: fn () -> i32 {
    raw := answer.as(^void)
    callback := raw.as(Callback)
    return callback()
}
¬
42
¬

¬
