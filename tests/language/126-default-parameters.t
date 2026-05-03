use std.io

add :: fn (a: i32, b: i32 = 20, c: i32 = a + b) => a + b + c

local_call :: fn (a: i32) {
    inner :: fn (x: i32, y: i32 = x + 3) => x * y
    prn($"inner {inner(a)}")
}

main :: fn () {
    prn($"one {add(2)}")
    prn($"two {add(2, 3)}")
    prn($"three {add(2, 3, 4)}")
    local_call(4)
}
¬
0
¬
one 44
two 27
three 9
inner 28

¬

¬

