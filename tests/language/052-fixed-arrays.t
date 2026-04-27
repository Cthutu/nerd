use std.io

main :: fn () -> i32 {
    xs :: [1, 2, 3]
    ys: [2]i32 = [4, 5]

    prn($"xs = {xs[0]}, {xs[2]}")
    prn($"xs array = {xs}")
    prn($"ys = {ys[1]}")
    prn($"ys array = {ys}")

    return xs[1] + ys[0] + ys[1]
}
¬
11
¬
xs = 1, 3
xs array = [1, 2, 3]
ys = 5
ys array = [4, 5]

¬
fn main
string.reset
$0 = array[i32:4, i32:5]
local ys = [2]i32:$0
$1 = string.start
string.append string:"xs = "
$3 = array[i32:1, i32:2, i32:3]
$4 = [3]i32:$3[i32:0]
string.append i32:$4
string.append string:", "
$5 = [3]i32:$3[i32:2]
string.append i32:$5
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
$6 = string.start
string.append string:"xs array = "
string.append [3]i32:$3
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$8 = string.start
string.append string:"ys = "
$10 = [2]i32:ys[i32:1]
string.append i32:$10
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$11 = string.start
string.append string:"ys array = "
string.append [2]i32:ys
$12 = string.finish $11
call fn(string)->void:prn, string:$12
string.reset
$13 = [3]i32:$3[i32:1]
$14 = [2]i32:ys[i32:0]
$15 = i32:$13 + i32:$14
$16 = [2]i32:ys[i32:1]
$17 = i32:$15 + i32:$16
return i32:$17
end
¬

