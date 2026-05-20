use std.io

read :: fn (value: Option[u8]) -> u8 {
    on value {
        Some(as byte) => {
            copied : u8 = byte
            return copied
        }
        None => {}
    }
    return 0
}

main :: fn () -> i32 {
    result := read(Some('x'))
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
