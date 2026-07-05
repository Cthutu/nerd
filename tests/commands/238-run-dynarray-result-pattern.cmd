main :: fn () -> i32 {
    bytes : [..]u8 = nil
    result : Result[[..]u8, string] = Ok(bytes)

    on result {
        Ok(output) => {
            status := output.count == 0
            output.free()
            on status => return 0
        }
        else => {
        }
    }

    return 1
}
¬
0
¬

¬
delete
