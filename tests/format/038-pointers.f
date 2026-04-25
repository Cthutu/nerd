use mod std.print

main :: fn(){
values:[3]i32=[10,20,30]
array_ptr:^[3]i32=^values
elem_ptr:^i32=^values[1]
literal_ptr:^[3]i32=^[1,2,3]
prn($"{array_ptr[0]} {elem_ptr[0]} {literal_ptr[0]}")
}
¬
use mod std.print

main :: fn () {
    values      : [3]i32  = [10, 20, 30]
    array_ptr   : ^[3]i32 = ^values
    elem_ptr    : ^i32    = ^values[1]
    literal_ptr : ^[3]i32 = ^[1, 2, 3]

    prn($"{array_ptr[0]} {elem_ptr[0]} {literal_ptr[0]}")
}
