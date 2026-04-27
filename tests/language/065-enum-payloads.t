use std.io

Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        None => 0
        Some(as x) => x
        Pair(as left, as right) => left + right
        Text(_) => 100
    }
}

main :: fn () -> i32 {
    a : Maybe = None
    b : Maybe = Some(5)
    c := Maybe.Pair(10, 20)
    d : Maybe = Text("hello")

    prn($"scores {score(a)} {score(b)} {score(c)} {score(d)}")

    return score(c)
}
¬
30
¬
scores 0 5 30 100

¬
fn score
param enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value
local $0 = i32:0
$4 = enum(0)
$5 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$4
branch.false bool:$5, L2
label L3
$0 = i32:0
jump L1
label L2
$8 = enum(1)
$9 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$8
branch.false bool:$9, L6
$10 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(1)
$11 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(1)
local score$x$on47 = i32:$11
label L7
$0 = i32:score$x$on47
jump L1
label L6
$14 = enum(2)
$15 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$14
branch.false bool:$15, L12
$16 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(2)
$17 = (i32,i32):$16.0
$18 = (i32,i32):$16.1
$19 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(2)
$20 = (i32,i32):$19.0
local score$left$on54 = i32:$20
$21 = (i32,i32):$19.1
local score$right$on57 = i32:$21
label L13
$22 = i32:score$left$on54 + i32:score$right$on57
$0 = i32:$22
jump L1
label L12
$24 = enum(3)
$25 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$24
branch.false bool:$25, L1
$26 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(3)
$27 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(3)
label L23
$0 = i32:100
label L1
return i32:$0
end
fn main
string.reset
$0 = enum(0)
local a = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$0
$1 = enum(1) i32:5
local b = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$1
$2 = tuple(i32:10, i32:20)
$3 = enum(2) (i32,i32):$2
local c = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$3
$4 = enum(3) string:"hello"
local d = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$4
$5 = string.start
string.append string:"scores "
$7 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:a
string.append i32:$7
string.append string:" "
$8 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:b
string.append i32:$8
string.append string:" "
$9 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:c
string.append i32:$9
string.append string:" "
$10 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:d
string.append i32:$10
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$11 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:c
return i32:$11
end
¬

