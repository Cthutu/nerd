use std.io

main :: fn () -> i32 {
    value: i32 = 1

    on yes => value = 7
    prn($"{value}")

    copy: i32 = value = 9
    prn($"{value} {copy}")

    return copy
}
¬
9
¬
7
9 9

¬
delete
¬
--llvm-backend
