use std.compress

main :: fn () -> i32 {
    compressed : [8]u8 = [120, 156, 3, 0, 0, 0, 0, 1]
    result := compressed[..].deflate_zlib()

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
