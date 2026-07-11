main :: fn () -> i32 {
    bytes : [..]u8 = nil
    result : [..]u8\string = bytes

    on result {
        output => {
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
