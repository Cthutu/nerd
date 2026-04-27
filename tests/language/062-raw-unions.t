use std.io

Value :: union {
    i i32
    f f32
}

main :: fn () -> i32 {
    a: Value = Value { i: 42 }
    b: Value = Value { f: 3.5 }

    prn($"i {a.i}")
    prn($"f {b.f}")

    return a.i
}
¬
42
¬
i 42
f 3.5

¬
fn main
string.reset
$0 = union(i: i32:42)
local a = union{i:i32,f:f32}:$0
$1 = union(f: f32:3.5)
local b = union{i:i32,f:f32}:$1
$2 = string.start
string.append string:"i "
$4 = union{i:i32,f:f32}:a.i
string.append i32:$4
$3 = string.finish $2
call fn(string)->void:prn, string:$3
string.reset
$5 = string.start
string.append string:"f "
$7 = union{i:i32,f:f32}:b.f
string.append f32:$7
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$8 = union{i:i32,f:f32}:a.i
return i32:$8
end
¬

