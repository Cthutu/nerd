use std.io

add :: fn (a: i32, b: i32 = 20, c: i32 = a + b) => a + b + c
scale :: fn (value: i32) => value * 10
with_call_default :: fn (value: i32 = scale(3)) => value + 1

local_call :: fn (a: i32) {
    inner :: fn (x: i32, y: i32 = x + 3) => x * y
    prn($"inner {inner(a)}")
}

main :: fn () {
    prn($"one {add(2)}")
    prn($"two {add(2, 3)}")
    prn($"three {add(2, 3, 4)}")
    prn($"call {with_call_default()}")
    local_call(4)
}
¬
0
¬
one 44
two 27
three 9
call 31
inner 28

¬

¬
