use std.io

Value::union{i i32 f f32 label string}

main::fn(){
v:Value=Value{i:42}
prn($"{v.i}")
}
¬
use std.io

Value :: union {
    i     i32
    f     f32
    label string
}

main :: fn () {
    v : Value = Value { i : 42 }
    prn($"{v.i}")
}
