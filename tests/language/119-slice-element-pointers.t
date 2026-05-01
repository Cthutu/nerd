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
$1 = ^[3]i32:fixed[i32:1]
local fixed_ptr = ^i32:$1
*^i32:fixed_ptr = i32:21
$2 = [3]i32:fixed[..]
local slice = []i32:$2
$3 = ^[]i32:slice[i32:2]
local slice_ptr = ^i32:$3
*^i32:slice_ptr = i32:32
local dyn = [..]i32:0
dynarray.push
dynarray.push
$4 = ^[..]i32:dyn[i32:1]
local dyn_ptr = ^i32:$4
*^i32:dyn_ptr = i32:7
local text = string:"abc"
$5 = ^string:text[i32:1]
local char_ptr = ^u8:$5
$6 = string.start
$8 = [3]i32:fixed[i32:1]
string.append i32:$8
string.append string:" "
$9 = []i32:slice[i32:2]
string.append i32:$9
string.append string:" "
$10 = [..]i32:dyn[i32:1]
string.append i32:$10
string.append string:" "
$11 = ^u8:char_ptr[usize:0]
string.append u8:$11
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$12 = [3]i32:fixed[i32:1]
$13 = []i32:slice[i32:2]
$14 = i32:$12 + i32:$13
$15 = [..]i32:dyn[i32:1]
$16 = i32:$14 + i32:$15
$17 = ^u8:char_ptr[usize:0]
$18 = cast u8:$17
$19 = i32:$16 + i32:$18
local result = i32:$19
dynarray.free
return i32:result
end
¬
