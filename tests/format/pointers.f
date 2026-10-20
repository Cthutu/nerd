use std.io

main :: fn(){
values:[3]i32=[10,20,30]
array_ptr:^[3]i32=^values
elem_ptr:^i32=^values[1]
literal_ptr:^[3]i32=^[1,2,3]

first:=array_ptr^[0]
prn($"{array_ptr[0]} {elem_ptr[0]} {literal_ptr[0]}")
}
¬
use std.io

main :: fn () {
    values      : [3]i32  = [10, 20, 30]
    array_ptr   : ^[3]i32 = ^values
    elem_ptr    : ^i32    = ^values[1]
    literal_ptr : ^[3]i32 = ^[1, 2, 3]

    first := array_ptr^[0]
    prn($"{array_ptr[0]} {elem_ptr[0]} {literal_ptr[0]}")
}
¬
main::fn()->i32{
ptr:^i32=nil
value:i32=7
ptr=^value
ptr^=9
ptr=nil
return value-9
}
¬
main :: fn () -> i32 {
    ptr   : ^i32 = nil
    value : i32  = 7

    ptr = ^value
    ptr^ = 9
    ptr = nil
    return value - 9
}
¬
main :: fn () {
items := [["quit", "q"]]
for group in items {
for item in group^{
on item^=>"quit"
}
}
}
¬
main :: fn () {
    items := [["quit", "q"]]
    for group in items {
        for item in group^ {
            on item^ => "quit"
        }
    }
}
¬
main :: fn () {
}

broken :: [
    ["n", "north"
]

move :: fn (state:^State, direction:string) {
}
¬
main :: fn () {
}

broken :: [
    ["n", "north"
]

move :: fn (state: ^State, direction: string) {
}
