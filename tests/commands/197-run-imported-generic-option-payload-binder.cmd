Key :: enum { A B }

accept :: fn (key: Key) -> i32 {
    return key.as(i32)
}

main :: fn () -> i32 {
    maybe: ?Key = Key.A
    return on maybe {
        code => accept(code) - Key.A.as(i32)
        else => 1
    }
}
¬
0
¬

¬
keep
¬

¬
run
