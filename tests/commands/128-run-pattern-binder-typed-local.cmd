use std.io

read :: fn (value: ?u8) -> u8 {
    on value {
        byte => {
            copied : u8 = byte
            return copied
        }
        else => {}
    }
    return 0
}

main :: fn () -> i32 {
    result := read('x')
    prn($"{result}")
    return result.as(i32)
}
¬
120
¬
120

¬
delete
¬
