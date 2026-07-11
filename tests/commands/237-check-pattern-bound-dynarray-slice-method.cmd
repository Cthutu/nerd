use std.compress

Container :: plex {
    chunks [..][]u8
}

make_container :: fn (compressed: []u8) -> Container\i32 {
    container := Container { chunks: nil }
    container.chunks.push(compressed)
    return container
}

main :: fn () -> i32 {
    compressed : [8]u8 = [120, 156, 3, 0, 0, 0, 0, 1]
    result_container := make_container(compressed[..])

    on result_container {
        value => {
            bytes : [..]u8 = nil
            direct : [..]u8\string = bytes
            on direct {
                output => {
                    status := output.count == 0
                    output.free()
                    on status => return 0
                }
                else => {
                }
            }
            value.chunks.free()
        }
        _! => {
        }
    }

    return 1
}
¬
0
¬

¬
delete
