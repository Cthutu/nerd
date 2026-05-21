use std.io

Buffer :: plex {
    bytes []u8
    count usize
    ok bool
}

make :: fn (capacity: usize) -> Buffer {
    return Buffer { bytes: temp_arena.alloc_bytes(capacity), count: 0, ok: yes }
}

push :: fn (buffer: ^Buffer, text: string) {
    buffer^.count += text.count
}

main :: fn () -> i32 {
    output := make(16)

    on output.ok => {
        push(^output, "hello")
    }

    prn($"count={output.count}")
    return output.count.as(i32)
}
¬
5
¬
count=5

¬
delete
¬
--llvm
