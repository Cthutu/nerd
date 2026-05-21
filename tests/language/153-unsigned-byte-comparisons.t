inner :: fn (a: i32, b: i32, c: i32) -> i32 {
    return a + b + c
}

outer :: fn (a: i32, b: i32) -> i32 {
    return a * b
}

main :: fn () {
    value := 65.as(u8)
    high := 0x80.as(u8)

    prn(on value < high => "lt" else "bad")
    prn(on high > value => "gt" else "bad")
    prn($"nested={outer(inner(1, 2, 3), 4)}")
    return on value <= value && high >= value => 0 else 1
}
¬
0
¬
lt
gt
nested=24

¬

¬
