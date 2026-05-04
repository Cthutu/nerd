use std.io

main :: fn () -> i32 {
    fixed: [3]i32 = [10, 20, 30]
    fixed_ptr: ^i32 = ^fixed[1]
    fixed_ptr^ = 21

    slice: []i32 = fixed[..]
    slice_ptr: ^i32 = ^slice[2]
    slice_ptr^ = 32

    dyn: [..]i32
    dyn.push(5)
    dyn.push(6)
    dyn_ptr: ^i32 = ^dyn[1]
    dyn_ptr^ = 7

    text := "abc"
    char_ptr: ^u8 = ^text[1]

    prn($"{fixed[1]} {slice[2]} {dyn[1]} {char_ptr^}")

    result := fixed[1] + slice[2] + dyn[1] + char_ptr^.as(i32)
    dyn.free()
    return result
}
¬
158
¬
21 32 7 98

¬
fn main
string.reset
$0 = array[i32:10, i32:20, i32:30]
local fixed = [3]i32:$0
$2 = ^[3]i32:fixed
$1 = ^^[3]i32:$2^[i32:1]
local fixed_ptr = ^i32:$1
*^i32:fixed_ptr = i32:21
$3 = [3]i32:fixed[..]
local slice = []i32:$3
$5 = ^[]i32:slice
$4 = ^^[]i32:$5^[i32:2]
local slice_ptr = ^i32:$4
*^i32:slice_ptr = i32:32
local dyn = [..]i32:0
dynarray.push
dynarray.push
$7 = ^[..]i32:dyn
$6 = ^^[..]i32:$7^[i32:1]
local dyn_ptr = ^i32:$6
*^i32:dyn_ptr = i32:7
local text = string:"abc"
$9 = ^string:text
$8 = ^^string:$9^[i32:1]
local char_ptr = ^u8:$8
$10 = string.start
$12 = [3]i32:fixed[i32:1]
string.append i32:$12
string.append string:" "
$13 = []i32:slice[i32:2]
string.append i32:$13
string.append string:" "
$14 = [..]i32:dyn[i32:1]
string.append i32:$14
string.append string:" "
$15 = ^u8:char_ptr[usize:0]
string.append u8:$15
$11 = string.finish $10
call fn(string)->void:prn, string:$11
string.reset
$16 = [3]i32:fixed[i32:1]
$17 = []i32:slice[i32:2]
$18 = i32:$16 + i32:$17
$19 = [..]i32:dyn[i32:1]
$20 = i32:$18 + i32:$19
$21 = ^u8:char_ptr[usize:0]
$22 = cast u8:$21
$23 = i32:$20 + i32:$22
local result = i32:$23
dynarray.free
return i32:result
end
init
input_buf = [256]u8:0
input_len = usize:0
end
¬
