wrap :: fn (value: i32) -> i32\Error { return value }
optional :: fn (value: i32) -> ?i32 { return value }
widen :: fn (value: i32) -> i64 { return value }
call :: fn () -> i64 { return narrow() }
inferred :: fn (value: i32) => value
¬
wrap :: fn (value: i32) -> i32\Error {
    return value
}

optional :: fn (value: i32) -> ?i32 {
    return value
}

widen :: fn (value: i32) -> i64 {
    return value
}

call :: fn () -> i64 {
    return narrow()
}

inferred :: fn (value: i32) => value
