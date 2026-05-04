use std.io

main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 30]
    array_ptr: ^[3]i32 = ^values
    elem_ptr: ^i32 = ^values[1]
    literal_ptr: ^[3]i32 = ^[1, 2, 3]

    prn($"array pointer = {array_ptr[0]}")
    prn($"elem pointer = {elem_ptr[0]}")
    prn($"literal pointer = {literal_ptr[0]}")

    return array_ptr[0][0] + elem_ptr[0] + literal_ptr[0][2]
}
¬
33
¬
array pointer = [10, 20, 30]
elem pointer = 20
literal pointer = [1, 2, 3]

¬
fn main
string.reset
$0 = array[i32:10, i32:20, i32:30]
local values = [3]i32:$0
$1 = ^[3]i32:values
local array_ptr = ^[3]i32:$1
$3 = ^[3]i32:values
$2 = ^^[3]i32:$3^[i32:1]
local elem_ptr = ^i32:$2
$5 = array[i32:1, i32:2, i32:3]
$4 = ^[3]i32:$5
local literal_ptr = ^[3]i32:$4
$6 = string.start
string.append string:"array pointer = "
$8 = ^[3]i32:array_ptr[i32:0]
string.append [3]i32:$8
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$9 = string.start
string.append string:"elem pointer = "
$11 = ^i32:elem_ptr[i32:0]
string.append i32:$11
$10 = string.finish $9
call fn(string)->void:prn, string:$10
string.reset
$12 = string.start
string.append string:"literal pointer = "
$14 = ^[3]i32:literal_ptr[i32:0]
string.append [3]i32:$14
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$15 = ^[3]i32:array_ptr[i32:0]
$16 = [3]i32:$15[i32:0]
$17 = ^i32:elem_ptr[i32:0]
$18 = i32:$16 + i32:$17
$19 = ^[3]i32:literal_ptr[i32:0]
$20 = [3]i32:$19[i32:2]
$21 = i32:$18 + i32:$20
return i32:$21
end
init
input_buf = [256]u8:0
input_len = usize:0
end
¬

