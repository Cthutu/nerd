use std.io

Point :: plex {
    x i32
    y i32
    name string
}

score_pair :: fn (pair: (i32, i32)) -> i32 {
    wanted :: 9
    return on pair {
        (0, _) => 100
        (1, as y) => y
        (as x, wanted) => x * 10
        else => 0
    }
}

score_point :: fn (point: Point) -> i32 {
    fallback :: "fallback"
    return on point {
        { name: "origin", x: as x } => x
        { x: 3, y: as y } => y * 10
        { name: fallback } => 50
        else => 0
    }
}

name_of :: fn (point: Point) -> string {
    return on point {
        { name: as name } => name
        else => ""
    }
}

main :: fn () -> i32 {
    prn($"pair zero {score_pair((0, 7))}")
    prn($"pair one {score_pair((1, 8))}")
    prn($"pair nine {score_pair((4, 9))}")
    prn($"pair else {score_pair((2, 3))}")
    prn($"point origin {score_point(Point { x: 2, y: 3, name: "origin" })}")
    prn($"point x {score_point(Point { x: 3, y: 4, name: "other" })}")
    prn($"point fallback {score_point(Point { x: 5, y: 6, name: "fallback" })}")
    prn($"point name {name_of(Point { x: 9, y: 10, name: "named" })}")
    return 0
}
¬
0
¬
pair zero 100
pair one 8
pair nine 40
pair else 0
point origin 2
point x 40
point fallback 50
point name named

¬
fn score_pair
param (i32,i32):pair
local $0 = i32:0
$4 = (i32,i32):pair.0
$5 = i32:$4 == i32:0
branch.false bool:$5, L2
$6 = (i32,i32):pair.1
$7 = (i32,i32):pair.0
$8 = (i32,i32):pair.1
label L3
$0 = i32:100
jump L1
label L2
$11 = (i32,i32):pair.0
$12 = i32:$11 == i32:1
branch.false bool:$12, L9
$13 = (i32,i32):pair.1
$14 = (i32,i32):pair.0
$15 = (i32,i32):pair.1
local score_pair$y$on51 = i32:$15
label L10
$0 = i32:score_pair$y$on51
jump L1
label L9
$18 = (i32,i32):pair.0
$19 = (i32,i32):pair.1
$20 = i32:$19 == i32:9
branch.false bool:$20, L16
$21 = (i32,i32):pair.0
local score_pair$x$on57 = i32:$21
$22 = (i32,i32):pair.1
label L17
$23 = i32:score_pair$x$on57 * i32:10
$0 = i32:$23
jump L1
label L16
$0 = i32:0
label L1
return i32:$0
end
fn score_point
param plex{x:i32,y:i32,name:string}:point
local $0 = i32:0
$4 = plex{x:i32,y:i32,name:string}:point.name
$5 = string:$4 == string:"origin"
branch.false bool:$5, L2
$6 = plex{x:i32,y:i32,name:string}:point.x
$7 = plex{x:i32,y:i32,name:string}:point.name
$8 = plex{x:i32,y:i32,name:string}:point.x
local score_point$x$on98 = i32:$8
label L3
$0 = i32:score_point$x$on98
jump L1
label L2
$11 = plex{x:i32,y:i32,name:string}:point.x
$12 = i32:$11 == i32:3
branch.false bool:$12, L9
$13 = plex{x:i32,y:i32,name:string}:point.y
$14 = plex{x:i32,y:i32,name:string}:point.x
$15 = plex{x:i32,y:i32,name:string}:point.y
local score_point$y$on110 = i32:$15
label L10
$16 = i32:score_point$y$on110 * i32:10
$0 = i32:$16
jump L1
label L9
$19 = plex{x:i32,y:i32,name:string}:point.name
$20 = string:$19 == string:"fallback"
branch.false bool:$20, L17
$21 = plex{x:i32,y:i32,name:string}:point.name
label L18
$0 = i32:50
jump L1
label L17
$0 = i32:0
label L1
return i32:$0
end
fn name_of
param plex{x:i32,y:i32,name:string}:point
local $0 = string:0
$4 = plex{x:i32,y:i32,name:string}:point.name
$5 = plex{x:i32,y:i32,name:string}:point.name
local name_of$name$on148 = string:$5
label L3
$0 = string:name_of$name$on148
jump L1
label L2
$0 = string:""
label L1
return string:$0
end
fn main
string.reset
$0 = string.start
string.append string:"pair zero "
$2 = tuple(i32:0, i32:7)
$3 = call fn((i32,i32))->i32:score_pair, (i32,i32):$2
string.append i32:$3
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$4 = string.start
string.append string:"pair one "
$6 = tuple(i32:1, i32:8)
$7 = call fn((i32,i32))->i32:score_pair, (i32,i32):$6
string.append i32:$7
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
$8 = string.start
string.append string:"pair nine "
$10 = tuple(i32:4, i32:9)
$11 = call fn((i32,i32))->i32:score_pair, (i32,i32):$10
string.append i32:$11
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$12 = string.start
string.append string:"pair else "
$14 = tuple(i32:2, i32:3)
$15 = call fn((i32,i32))->i32:score_pair, (i32,i32):$14
string.append i32:$15
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$16 = string.start
string.append string:"point origin "
$18 = plex(x: i32:2, y: i32:3, name: string:"origin")
$19 = call fn(plex{x:i32,y:i32,name:string})->i32:score_point, plex{x:i32,y:i32,name:string}:$18
string.append i32:$19
$17 = string.finish $16
call fn(string)->void:prn, string:$17
string.reset
$20 = string.start
string.append string:"point x "
$22 = plex(x: i32:3, y: i32:4, name: string:"other")
$23 = call fn(plex{x:i32,y:i32,name:string})->i32:score_point, plex{x:i32,y:i32,name:string}:$22
string.append i32:$23
$21 = string.finish $20
call fn(string)->void:prn, string:$21
string.reset
$24 = string.start
string.append string:"point fallback "
$26 = plex(x: i32:5, y: i32:6, name: string:"fallback")
$27 = call fn(plex{x:i32,y:i32,name:string})->i32:score_point, plex{x:i32,y:i32,name:string}:$26
string.append i32:$27
$25 = string.finish $24
call fn(string)->void:prn, string:$25
string.reset
$28 = string.start
string.append string:"point name "
$30 = plex(x: i32:9, y: i32:10, name: string:"named")
$31 = call fn(plex{x:i32,y:i32,name:string})->string:name_of, plex{x:i32,y:i32,name:string}:$30
string.append string:$31
$29 = string.finish $28
call fn(string)->void:prn, string:$29
string.reset
return i32:0
end
¬

