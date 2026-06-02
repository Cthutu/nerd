Key :: enum { A B }

accept :: fn (key: Key) -> i32 {
    return key.as(i32)
}

main :: fn () -> i32 {
    maybe: Option[Key] = Some(Key.A)
    return on maybe {
        Some(code) => accept(code) - Key.A.as(i32)
        None => 1
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
