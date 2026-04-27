use std.io

main :: fn(){
values:[5]i32=[10,20,30,40,50]
all:[]i32=values[..]
middle:[]i32=values[1..4]
from_start:[]i32=values[..3]
to_end:[]i32=values[2..]
literal:[]i32=[1,2,3][..]
prn($"{all} {middle.count} {middle.data[0]}")
}
¬
use std.io

main :: fn () {
    values     : [5]i32 = [10, 20, 30, 40, 50]
    all        : []i32  = values[..]
    middle     : []i32  = values[1..4]
    from_start : []i32  = values[..3]
    to_end     : []i32  = values[2..]
    literal    : []i32  = [1, 2, 3][..]

    prn($"{all} {middle.count} {middle.data[0]}")
}
