use std.io
main::fn()->i32{
values:[2]i32=[1,2]
ptr:^i32=^values[1]
for ^item in values[..]{
prn($"{item^}")
}
return ptr^
}
¬
use std.io

main :: fn () -> i32 {
    values : [2]i32 = [1, 2]
    ptr    : ^i32   = ^values[1]

    for ^item in values[..] {
        prn($"{item^}")
    }
    return ptr^
}
