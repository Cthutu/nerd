main :: fn () -> i32 {
    value: i32 = 7
    ptr: ^i32 = nil

    ptr = ^value
    ptr^ = 9
    ptr = nil

    return value - 9
}
¬
0
¬

¬

¬
