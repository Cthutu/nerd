use std.io

value :: fn (x: i32) => x

pair :: fn () -> (i32, i32) {
    return (value(10), value(20))
}

main :: fn () {
    result := pair()
    prn($"{result.0}x{result.1}")
}
¬
0
¬
10x20

¬
