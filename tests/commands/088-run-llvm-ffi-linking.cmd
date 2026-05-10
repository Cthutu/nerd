use std.io

libm :: "m"
square_root :: ffi libm sqrt (f64) -> f64

main :: fn () -> i32 {
    value := square_root(9.0)
    prn($"sqrt {value}")
    return 0
}
¬
0
¬
sqrt 3

¬
delete
¬
