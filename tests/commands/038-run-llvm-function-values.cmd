use std.io

adder := fn (a: i32, b: i32) => a + b

main :: fn () -> i32 {
    local_result := add(20, 22)
    add :: fn (a: i32, b: i32) => a + b

    nested :: fn (a: i32, b: i32) => a + b

    prn($"global={adder(20, 22)}")
    prn($"local={local_result}")
    prn($"nested={nested(5, 6)}")
    return adder(20, 22)
}
¬
42
¬
global=42
local=42
nested=11

¬
delete
¬
