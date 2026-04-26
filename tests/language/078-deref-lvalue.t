use std.print

main :: fn () -> i32 {
    value := 1
    ptr: ^i32 = ^value

    ptr^ = 7

    prn($"{value} {ptr^}")
    return value - 7
}
¬0¬7 7

¬¬
