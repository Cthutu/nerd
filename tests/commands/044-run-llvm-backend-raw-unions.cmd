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
delete
¬
--llvm-backend
