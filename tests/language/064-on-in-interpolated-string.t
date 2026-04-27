use std.io

Colour :: enum { RED GREEN BLUE }

main :: fn () {
    colour : Colour = RED

    prn($"Colour = {on colour { RED => "red" GREEN => "green" BLUE => "blue" }}")
}
¬
0
¬
Colour = red

¬
fn main
string.reset
$0 = enum(0)
local colour = enum{RED,GREEN,BLUE}:$0
$1 = string.start
string.append string:"Colour = "
local $3 = string:0
$7 = enum(0)
$8 = enum{RED,GREEN,BLUE}:colour == enum{RED,GREEN,BLUE}:$7
branch.false bool:$8, L5
label L6
$3 = string:"red"
jump L4
label L5
$11 = enum(1)
$12 = enum{RED,GREEN,BLUE}:colour == enum{RED,GREEN,BLUE}:$11
branch.false bool:$12, L9
label L10
$3 = string:"green"
jump L4
label L9
$14 = enum(2)
$15 = enum{RED,GREEN,BLUE}:colour == enum{RED,GREEN,BLUE}:$14
branch.false bool:$15, L4
label L13
$3 = string:"blue"
label L4
string.append string:$3
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
return i32:0
end
¬

